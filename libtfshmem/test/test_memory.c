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

int main() {

	uint64_t size = 100ULL*1024*1024*1024;
	uint64_t buf_size = 100ULL*1024*1024;
	void* ea;
	void* buf_ptr;
	void* buf2_ptr;
	char ch;
	int i;
	unsigned long long int sum = 0;
	unsigned long long int val = 0;
	unsigned long long int zeroes = 0;

	printf("\nTesting initializing OCAPI shared memory pool\n");


	/*
	 * Initialize the memory pool used to allocate buffers from user applications.
	 * The only info needed at this stage is the size of the pool itself.
	 * */
	if (tf_memory_init(size) == -1){
		printf ("Error initializing shared memory pool");
		return -1;
	}

	/*
	 * Retrieve the base ea of the pool. This value must be shared with the counterpart
	 * on the compute node.
	 * */
	ea = tf_get_pool_ea();
	printf("\nOCAPI shared memory pool initialised correctly at ea %p\n", ea);

	/*
	 * Carves out a 100MB buffer from the main pool. The resulting ea is to be share
	 * with the application on the memory node for mapping
	 * */
	buf_ptr = tf_buffer_alloc(buf_size);
	if (buf_ptr == NULL){
		printf ("Error allocating buffer");
		return -1;
	}

	printf("\n %" PRIu64 "bytes allocated in a buffer at ea %p\n", buf_size, buf_ptr);

	/*
	 * Carves out a 200MB buffer from the main pool. The resulting ea is to be share
	 * with the application on the memory node for mapping
	 * */
	buf2_ptr = tf_buffer_alloc(2*buf_size);
	if (buf_ptr == NULL){
		printf ("Error allocating buffer");
		return -1;
	}

	printf("\n %" PRIu64 "bytes allocated in a buffer at ea %p\n", 2*buf_size, buf2_ptr);

	printf("\nConfigure compute node with the pool and buffers effective address\n");
	printf("\nPress any key to exit when mapping is completed on the compute side\n");
	ch = getchar();

	for (i=0; i < (buf_size)/(sizeof (unsigned int)); i++){
		val = ((unsigned int *)buf_ptr)[i];
		if (val == 0)
			zeroes += 1;
		else
			sum += val;
	}	

	printf("\nBuffer 1 sum is %llu, zeroes are %llu", sum, zeroes);

	sum = 0;
	zeroes = 0;
	for (i=0; i < (2 * buf_size)/(sizeof (unsigned int)); i++){
		val = ((unsigned int *)buf2_ptr)[i];
		if (val == 0)
			zeroes += 1;
		else
			sum += val;
	}	

	printf("\nBuffer 2 sum is %llu, zeroes are %llu", sum, zeroes);

}
