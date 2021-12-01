## LibTFShmem: a library enabling shared memory on Thymesisflow

This library enables creating a shared memory segment across processes running on two
machines interfaced via ThymesisFlow. The idea is that the two processes will be able
to map a buffer in memory and access it concurrently.
ALthough this is possible, there are a few caveats to take into consideration:
- memory accesses across the two machines are neither coherent nor consistent by nature.
  It will be up-to the programmer to make sure memory is properly handled.
- (unexpectedly) The data-cache is your worst enemy here. The two sides sharing memory
  might as well keep accessing a local copy of the shared data in their data cache as
  no cache-coherence protocol is in place across the two machines. It will be again up-to
  the programmer to make sure data is casted out from one processor cache before the
  other side can be aware of it. There are sample applications showing how to deal with
  the cache, have a look [here](libtfshmem/test).
  - Do not try make the mapping un-cacheable by hacking the mmap in [here](libtfshmem/kernel_module),
    as this would make the side borrowing memory (a.k.a. the compute) fail miserably at
    the first memory access. ThymesisFlow is only supporting 128B memory transactions and
    and disabling the data cache does not help enforcing this.

Having said that,have fun experimentign with shared memory on thymesisflow. When using this
library there's no need for you to start the thymesisflow-agent or to use the CLI.
Have a look at the examples [here](libtfshmem/test). Where, `memory` is the side
initializing and lending the physical memory, while compute is the side `borrowing`
the memory.

### How-to compile the library
First make sure you have compiled `libthymesisflow`. Then just run `make` in this folder.
There's a `DEBUG` flag in the Makefile that is set to 1 by default. Set it to 0 if you want
a less verbose but **less informative** run.
