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
 *         Panagiotis Koutsovasilis - koutsovasilis.panagiotis1@ibm.com
 */

#ifndef CUSTOM_MM_H
#define CUSTOM_MM_H

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

#define BLOCK_T_ALIGNED (ALIGN(sizeof(block_t), 8)) // block_t aligned 8 bytes

#define PAGE_SIZE 64*1024

typedef struct _block_t
{
    unsigned long len;
    struct _block_t *next;
    struct _block_t *prev;
} block_t;


void init_custom_mm(void *base_addr, uint64_t size);
void fini_custom_mm();
void * custom_malloc(size_t size);
void * custom_malloc_aligned(size_t alignment, size_t size);
void custom_free(void *ptr);

#endif

