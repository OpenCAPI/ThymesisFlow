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

#ifndef TF_SHMEM_H
#define TF_SHMEM_H

#define COMPUTE_PORT 0x2
#define MEMORY_PORT 0x1
#define AFU_NAME "IBM,RMEM"
/*
 * This is the physical address whete the ThymesisFlow OpenCAPI accelerator
 * is mapped and represents the base physical address for the attached 
 * disaggregated memory pool.
 */
#define OCAPI_PHYS_BASE 0x2000000000000

struct tf_shmem_state {
	uint64_t ea;
	uint64_t pool_size;
	int memory_node;
};

#endif

