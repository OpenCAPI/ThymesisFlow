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
#ifndef _TFSHMEM_H_
#define _TFSHMEM_H_

#define OCAPI_PHY_BASE  0x2000000000000

int tfshmem_open(struct inode *inode, struct file *filp);
int tfshmem_release(struct inode *inode, struct file *filp);
int tfshmem_mmap (struct file *filp, struct vm_area_struct *vma);

#endif


