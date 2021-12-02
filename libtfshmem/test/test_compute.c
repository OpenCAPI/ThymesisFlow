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
#include "tf_shmem_api.h"

/* POWER9 cacheline size
 * TODO: make this more robust*/
#define CACHELINE_SIZE 0x80
#define CACHELINE_MASK (CACHELINE_SIZE - 1)


#define FLUSH_LINE(addr) \
	asm volatile ( \
		"dcbf 0x0,%0\n" \
		: \
		: "b"(addr) \
		: "memory" \
	)


#define FLUSH_LINE_SYNC(addr) \
	asm volatile ( \
		"dcbf 0x0,%0\n" \
		"sync\n" \
		: \
		: "b"(addr) \
		: "memory" \
	)


static inline void flush_memory_block (void *addr, uint64_t size, int sync) {
	uint64_t start_addr = (uint64_t) addr;
	uint64_t end_addr = (start_addr + size);
	unsigned int count = 0;
	printf("\nstarting flusshing buffer @ address: %" PRIx64 " of size %" PRIu64, start_addr, size);

	if ((start_addr & CACHELINE_MASK) != 0) {
		if (sync)
			FLUSH_LINE_SYNC(start_addr);
		else {
//			printf("\n#%u flushing line @ address: %" PRIx64, count, start_addr);
			fflush(stdout);
			FLUSH_LINE(start_addr);
		}
		start_addr += CACHELINE_SIZE - (start_addr & CACHELINE_MASK);
	}

	for ( ; start_addr < end_addr; start_addr += CACHELINE_SIZE, count++){
		if (sync)
			FLUSH_LINE_SYNC(start_addr);
		else {
//			printf("\n#%u flushing line @ address: %" PRIx64, count, start_addr);
			fflush(stdout);
			FLUSH_LINE(start_addr);
		}
	}
}

int main() {

	uint64_t pool_size = 100ULL*1024*1024*1024;
	uint64_t pool_ea;
	uint64_t buf_size = 100ULL*1024*1024;
	uint64_t buf_ea;
	void *buf_ptr;
	int result;
	int i;
	unsigned long long int sum = 0;

	printf("\nClient test waiting for base ea of OCAPI pool to continue with initialization: ");
	result = scanf("%" PRIx64, &pool_ea);

	printf("\nRead: %" PRIx64 ", continuing initialization\n", pool_ea);

	/*
	 * Initialize the TF shared memory library by providing the ea of the OCAPI memory pool
	 * and the size of the pool itself. The ea is not valid here (compute node) as it is
	 * an address generated at the memory node side. However this is needed to configure the
	 * ThymesisFlow design.
	 * The ea is usually provided, somehow, by the memory node.
	 * */
	if (tf_compute_init((void *)pool_ea, pool_size) == -1){
		printf ("Error initializing shared memory pool");
		return -1;
	}
	printf("\nOCAPI shared memory initialized correctly");

	printf("\nClient test waiting for buffer base ea to continue with testing mapping it: ");
	result = scanf("%" PRIx64, &buf_ea);
	printf("\nRead: %" PRIx64 ", continuing initialization", buf_ea);

	/*
	 * Here we map a first 100MB buffer allocated at the memory node side. Again the ea is not valid here
	 * and it is used only to compute the offset from the main pool start.
	 * */
	buf_ptr = tf_buffer_map((void *)buf_ea, buf_size);
	if (buf_ptr == NULL){
		printf ("Error mapping buffer");
		return -1;
	}

	printf("\n %" PRIu64 "bytes mapped in a buffer at ea %p", buf_size, buf_ptr);
	for (i=0; i < buf_size/(sizeof (unsigned int)); i++) {
		((unsigned int *)buf_ptr)[i] = i;
		sum += i;
	}
	printf("\nBuffer 1 sum is %llu", sum);

	flush_memory_block(buf_ptr, buf_size, 0);

	tf_buffer_unmap(buf_ptr, buf_size);


	printf("\nClient test waiting for second buffer base ea to continue with testing mapping it: ");
	result = scanf("%" PRIx64, &buf_ea);
	printf("\nRead: %" PRIx64 ", continuing initialization\n", buf_ea);

	/*
	 * Here we map a second 100MB buffer allocated at the memory node side. Again the ea is not valid here
	 * and it is used only to compute the offset from the main pool start.
	 * */
	buf_ptr = tf_buffer_map((void *)buf_ea, 2*buf_size);
	if (buf_ptr == NULL){
		printf ("Error mapping buffer");
		return -1;
	}

	sum =0;
	printf("\n %" PRIu64 "mapped in a buffer at ea %p\n", 2*buf_size, buf_ptr);
	for (i=0; i < (2*buf_size)/(sizeof (unsigned int)); i++) {
		((unsigned int *)buf_ptr)[i] = (2*buf_size)/(sizeof (unsigned int)) - i;
		sum += (2*buf_size)/(sizeof (unsigned int)) - i;
	}
	printf("\nBuffer 2 sum is %llu", sum);

	flush_memory_block(buf_ptr, buf_size, 1);

	tf_buffer_unmap(buf_ptr, 2 * buf_size);

	if (tf_shmem_teardown() == -1) { 
		printf("Failed tearing down tfshmem library");
	}

}
