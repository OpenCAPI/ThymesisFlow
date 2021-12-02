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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include "tfshmem.h"

#define DRIVER_NAME "tf_shmem"
#define TFSHMEM_DEVICE_NAME "tfshmem"
#define TFSHMEM_DEV_COUNT 1

static int major;
static dev_t first = 0;
static struct class *cl = NULL;
static struct cdev mm_dev;
static struct device *dev;

struct file_operations mm_fops = {
	.open = tfshmem_open,
	.owner = THIS_MODULE,
	.release = tfshmem_release,
	.mmap = tfshmem_mmap,
};

/*
 * I want to use the same module for client (compute node) and server (memory node).
 * Users will set this param while probing the module and I'll use it just to make sure
 * nothing illegal is executed.
 *
 */
int client_mode = 0;
module_param(client_mode, int, 0);

static char *remap_devnode(struct device *dev, umode_t *mode) {
	if (!mode)
		return NULL;
	*mode = 0666;
	return NULL;
}

static int __init tfshmem_driver_register(void) {
	major = alloc_chrdev_region(&first, 0, TFSHMEM_DEV_COUNT, DRIVER_NAME);

	if(major < 0) {
	    pr_err("Cannot allocate major and minor numbers\n" );
	    return -1;
	}

	if ( (cl = class_create( THIS_MODULE, "tf" ) ) == NULL ) {
	    pr_err("Class creation failed\n" );
	    unregister_chrdev_region(first, TFSHMEM_DEV_COUNT);
	    return -1;
	}

	cl->devnode = remap_devnode;

	return 0;
}

static int __init tfshmem_device_register(char *device_name, struct file_operations *fops,

		dev_t first, struct cdev *c_dev) {
	if ((dev = device_create( cl, NULL, first, NULL, device_name)) == NULL ) {
	    pr_err("%s device creation failed\n", device_name);
	    return -1;
	}

	cdev_init(c_dev, fops);

	if (cdev_add(c_dev, first, 1) == -1) {
	    pr_err("%s device addition failed\n", device_name);
	    device_destroy(cl, first);
	    return -1;
	}

	pr_info("\"%s\" character device successfully created\n", device_name);

	return 0;
}

static void tfshmem_driver_unregister(void) {

	if (mm_dev.dev != 0) {
	    device_destroy(cl, first);
	}

	if (cl != NULL) {
	    class_destroy(cl);
	}

	if (first != 0) {
	    unregister_chrdev_region(first, TFSHMEM_DEV_COUNT);
	}
}

int __init tf_shmem_init(void) {

	int ret;
	pr_info("Starting TFSHMEM device driver initialization");
	if ((ret = tfshmem_driver_register())) {
		goto except_fail;
	}

	if ((ret = tfshmem_device_register(TFSHMEM_DEVICE_NAME, &mm_fops, first, &mm_dev))) {
		goto except_fail;
	}

except_fail:
	if (ret < 0) {
		tfshmem_driver_unregister();
	}

	return ret;
}

void __exit module_teardown(void) {
	tfshmem_driver_unregister();
}


module_init(tf_shmem_init);
module_exit(module_teardown);
MODULE_LICENSE("GPL");
