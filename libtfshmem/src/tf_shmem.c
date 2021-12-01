/*
 * Copyright 2021 International Business Machines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Christian Pinto - christian.pinto@ibm.com
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "thymesisflow.h"
#include "utils.h"
#include "tf_shmem.h"
#include "custom_mm.h"


static struct tf_shmem_state state = {.ea = 0x0, .pool_size = 0, .memory_node = -1};

/*
 * 0 - compute node configuration
 * 1 - memory node configuration
 * */
static bool is_initialized() {
	return state.memory_node != -1 ;
}

bool tf_initialized(void) {
	return is_initialized();
}

void * tf_get_pool_ea(void) {
	if (state.memory_node !=1){
		DBG_PRINT("MI-SHMEM not initialized or wrong mode");
		return NULL;
	}
	return (void*) state.ea;
}

int tf_compute_init(void *ea, uint64_t pool_size) {
	iport_list ports;
	int ret = 0;

	if (is_initialized()){
		DBG_PRINT("Library already initialized");
		return -2;
	}

	DBG_PRINT("ea %p, size %" PRIu64, ea, pool_size);
	ports.nport = COMPUTE_PORT;
	ports.next = NULL;

	ret = attach_compute("0x1", AFU_NAME, &ports, (uint64_t)ea, pool_size, 1);
	if (ret != ATTACH_OK)
		ret = -1;

	DBG_PRINT("MI compute node initialization successfull");

	state.ea = (uint64_t) ea;
	state.pool_size = pool_size;

	state.memory_node = 0;

	//Christian: I have noticed that if the memory starts being used right away after
	// initial configuration the FPGAs fail. This is something we have seen in the past
	// and it is due to the training phase that takes place between aurora channels.
	// This value is totally arbitrary!!!
	sleep(5);

	return ret;
}

int tf_memory_init(uint64_t pool_size) {
	void *ptr;
	iport_list ports;
	int ret = 0;

	if (is_initialized()){
		DBG_PRINT("Library already initialized");
		return -2;
	}

	ports.nport = MEMORY_PORT;
	ports.next = NULL;

	ret = attach_memory("0x1", AFU_NAME, &ports, pool_size, (uint64_t *)&ptr);

	if (ret != ATTACH_OK) {
		printf("\nError attaching memory");
		return ret;
	}

	//TODO: initialize memory allocator

	DBG_PRINT("MI memory node initialization successfull");

	state.ea = (uint64_t)ptr;
	state.pool_size = pool_size;

	/*
	 * Initialize the custom memory allocator used to carve out buffers out of
	 * the main memory pool.
	 * Please note, this is a VERY VERY VERY basic allocator.
	 * */
	init_custom_mm((void *)ptr, pool_size);

	state.memory_node = 1;
	return ret;
}

int tf_shmem_teardown(void) {
	int ret;

	if (!is_initialized()) {
		DBG_PRINT("Library is not initialized");
		return 0;
	}


	ret = state.memory_node ? detach_memory("0x1") : detach_compute("0x1");
	if (ret != DETACH_OK) {
		DBG_PRINT("Error detaching %s node", state.memory_node ? "memory" : "compute");
		return -1;
	}

	state.ea = 0x0;
	state.pool_size = 0;
	state.memory_node = -1;
	
	return 0;
}

void * tf_buffer_alloc(uint64_t buffer_size) {
	/*
	 * This function carves out a buffer of requested size from the main memory pool
	 * initialized with tf_memory_init(). The function returns a pointer to this buffer.
	 * */
	void *ptr;

	if (!is_initialized()) {
		DBG_PRINT("not initialized!");
		return NULL;
	}

	if (!state.memory_node) {
		DBG_PRINT("Library initialized in compute node mode, can't allocate buffer");
		return NULL;
	}

	//TODO: Implement the actual function that allocates a buffer from the main pool
	if((ptr = custom_malloc(buffer_size)) == NULL) {
		DBG_PRINT("Error carving buffer out from main pool");
		return NULL;
	}

	DBG_PRINT("Allocated %" PRIu64 " bytes @ %p", buffer_size, ptr);
	
	return ptr;

}

static void * _buf_map_mmap (off_t offset, uint64_t size) {
	int fd;
	void *ret;
	if ((fd = open("/dev/tfshmem", O_RDWR|O_SYNC)) < 0 ) {
		printf("\nError opening /dev/mem.");
		close(fd);
		return NULL;
	}

	DBG_PRINT("Trying mapping buffer of size: %" PRIu64 " with offset ea %.16lx", size, offset);

	if ((ret = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset)) == (void *) -1) {
		printf("\n%s - mmap failed with error: %s\n", __func__, strerror(errno));
	}

	close(fd);

	return ret;
}

void* tf_buffer_map(void *ea, uint64_t buffer_size) {

	off_t buf_offset;
	off_t buf_phys_base;
	void *addr;

	if (ea == NULL){
		DBG_PRINT("ea is NULL!!");
		return NULL;
	}

	if (!is_initialized()) {
		DBG_PRINT("not initialized!");
		return NULL;
	}

	if (state.memory_node) {
		DBG_PRINT("Library initialized in memory node mode, can't map buffer");
		return NULL;
	}

	/*
	 * Assumption: the counterpart has sent over the base ea of this buffer from
	 * its perspective.
	 * Steps:
	 *  1- Compute buffer offset from memory pool start: We can do this by subtracting
	 *     the base ea of the pool from the buffer base ea.
	 *  2- Compute buffer physical memory address from compute node perspective
	 *  3- Map base physical address to this process memory map. 
	 * */
	buf_offset = (uint64_t)ea - (uint64_t)state.ea;
	buf_phys_base = buf_offset;
	
	DBG_PRINT ("buf_offset: %.16lX, buf_phys_base: %.16lX, size: %" PRIu64, buf_offset, buf_phys_base, buffer_size);

	addr = _buf_map_mmap(buf_phys_base, buffer_size);

	if(addr == (void *)-1) {
		DBG_PRINT("Mapping the buffer failed");
		return NULL;
	}

	if (mlock(addr, buffer_size) < 0) {
		// error mem lock ERR_MLOCK
		DBG_PRINT("cannot mlock memory for buffer");
		return NULL;
	}

	return addr;
}

int tf_buffer_unmap(void *buf, size_t length) {

	if (!is_initialized()) {
		DBG_PRINT("not initialized!");
		return -1;
	}

	if (state.memory_node) {
		DBG_PRINT("This is a memory node, you can't invoke this function");
		return -1;
	}

	return munmap (buf, length);
}

void tf_buffer_free(void * buf) {

	if (!is_initialized()) {
		DBG_PRINT("not initialized!");
		return;
	}

	if (!state.memory_node) {
		DBG_PRINT("This is a compute node, you can't invoke this function");
		return;
	}

	/*
	 * This function is currently doing nothing as we are not taking care of actually
	 * freeing a buffer carved out from the main memory pool
	 * */
	custom_free(buf);

	return;
}
