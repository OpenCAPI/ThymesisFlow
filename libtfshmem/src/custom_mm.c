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

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include "custom_mm.h"
#include "utils.h"

static void *base_address = NULL;
static void *next = NULL;
static long unsigned PREALLOC_SIZE = 0;
static bool initialized = false;

void custom_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    ptr=NULL;
}

void *custom_malloc(size_t size)
{
    if (size == 0 || !initialized){
        return NULL;
    }

    void *ret;
    size = ALIGN(size, PAGE_SIZE);

    if (next == NULL){
	    ret = base_address;
	    next = base_address + size;
    }
    else {
	    ret = next;
	    next = (char*)next + size;
    }

    DBG_PRINT("size: %.16lX, next %p, ret %p", size, next, ret);
    return ret;

}

void * custom_malloc_aligned(size_t alignment, size_t size) {
	void * memptr;
	if (!initialized) {
		return NULL;
	}
    size = ALIGN(size, alignment);
    memptr = custom_malloc(size);

	if (memptr == NULL) {
		DBG_PRINT("custom malloc failed");
	}

	return memptr;
}

void init_custom_mm(void *base_addr, uint64_t size) {
	base_address = base_addr;
	PREALLOC_SIZE = size;
	initialized = true;
}

void fini_custom_mm() {
	base_address = NULL;
	PREALLOC_SIZE = 0;
	initialized = false;
}
