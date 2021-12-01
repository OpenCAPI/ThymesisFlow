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

#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "tfshmem.h"

int tfshmem_open(struct inode *inode, struct file *filp) {

	return 0;
}

int tfshmem_release(struct inode *inode, struct file *filp) {

	return 0;
}


int tfshmem_mmap (struct file *filp, struct vm_area_struct *vma) {

	int ret;
	unsigned long size, addr, pfn;

	size = vma->vm_end - vma->vm_start;
	pfn = vma->vm_pgoff + (OCAPI_PHY_BASE >> PAGE_SHIFT);
	addr = vma->vm_start;

	pr_debug("size: %.16lx, pfn: %.16lx, addr, %.16lx", size, pfn, addr);
	
	/*
	 * This is mapping the contiguous physical addres space window where the shared buffer
	 * is located into the virtual address space of the calling process. Mapping is 1-to-1 as
	 * as it is both virtually and physically contiguous.
	 */
	if ((ret = remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot)) < 0) {
		printk("remap_pfn_range failed\n");
		return ret;
	}

	pr_debug("Mapping to user space successful\n");
	
	return 0;
}
