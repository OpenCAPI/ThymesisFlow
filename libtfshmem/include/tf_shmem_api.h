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

#ifndef TF_SHMEM_API_H
#define TF_SHMEM_API_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the memory inception interface for the compute node side.
 * This requires the effective address (EA) one the memory side to be provided as input.
 * The need for the EA is not a requirement of Memory Inception but rather linked to how the
 * ThymesysFlow prototype works.
 * arguments:
 *  ea: effective address at which the main pool is mapped on the memory side.
 *  pool_size: size of the mail remote memory pool in bytes.
 *
 * return value:
 * This function returns 0 in case of success or -1 in case of error and -2 if the library
 * is already initialized.
 */
int tf_compute_init(void *ea, uint64_t pool_size);

/**
 * Initialize the memory inception interface for the memory node side.
 * This method allocates a memory pool of the required size and makes it available for
 * remote access.
 * arguments:
 *  pool_size: size of the mail remote memory pool in bytes.
 *
 * return value:
 * This function returns 0 in case of success or -1 in case of error and -2 if the library
 * is already initialized.
 */
int tf_memory_init(uint64_t pool_size);

/**
 * Tear down the tf_shmem library. The function takes care or resetting the state and FPGAs.
 *
 * return value:
 * 0 in case of success -1 otherwise.
 */
int tf_shmem_teardown(void);

/**
 * Carve out a buffer of the requested size from the main memory pool. This function can only be invoked
 * on the memory node.
 *
 * arguments:
 *  buffer_size: the size in bytes of the buffer to be allocated
 *
 * return value:
 *  this function return NULL in case of error or a valid pointer otherwise.
 */
void* tf_buffer_alloc(uint64_t buffer_size);

/**
 * Map a buffer from the remote memory pool. This function is to be invoked on the compute node only.
 * arguments:
 *  ea: the EA of buffer base on the memory node side.
 *  buffer_size: the size in bytes of the buffer to be mapped.
 *
 * return value:
 *  this function return NULL in case of error or a valid pointer otherwise.
 */
void* tf_buffer_map(void *ea, uint64_t buffer_size);

/**
 * Returns the effective address of the main shared memory pool
 */
void * tf_get_pool_ea(void);

/**
 * Returns true if the library is initialized.
 * Can be invoked from both memory and compute node.
 */
bool tf_initialized(void);

/**
 * Frees a buffer allocated from the pool. This should only be invoked by a memory node
 */
void tf_buffer_free(void *buf);

/**
 * Unmaps a buffer from a compute note memory map. SHould only be invoked by a compute node.
 *
 * return value:
 * 0 in case of success, -1 otherwise.
 */
int tf_buffer_unmap(void *buf, size_t length);

#ifdef __cplusplus
}
#endif

#endif //#ifndef TF_SHMEM_H
