/// @file thymesisflow.h
#if !defined(THYMESISFLOW_H)
#define THYMESISFLOW_H

#include "connection.h"
#include "errcode.h"
#include "iport.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "ty_afu.h"

#ifndef MOCK
#include "libocxl.h"
#endif

#define GLOBAL_MMIO_REG_COUNT (0x300 / 8)

#define CTRL_REG 0x000

#define INIT_PROBE_ADDR 0x2000000000000

#define AFU_NAME "IBM,RMEM"

#define MEMBLOCK_SIZE 0x10000000

#define PROBE_PATH "/sys/devices/system/memory/probe"

#define MEMORY_PATH "/sys/devices/system/memory"

/**
 *  Setup AFU and allocate memory
 *  Note: ports are passed but not needed in this current hw implementation
 *
 * @param[in] circuit_id: circuit identifier
 * @param[in] afu_name: AFU name
 * @param[in] iport_list: list of ports to be used for the attachemnt (Currently
 * single port supported)
 * @param[in] size: size of memory to be allocated
 * @param[in] eaptr: effective address to return to the compute node for memory
 * attachment #param[out] error_code
 */
int attach_memory(const char *circuit_id, const char *afu_name,
                  iport_list *ports, const uint64_t size, uint64_t *eaptr);

/**
 *  Setup AFU and attach memory
 *
 * @param[in] circuit_id: circuit identifier
 * @param[in] afu_name: AFU name
 * @param[in] iport_list: list of ports to be used for the attachemnt (Currently
 * single port supported)
 * @param[in] effective_addr: address passed the AFU to attach the remove memory
 * @param[in] size: size of memory to be allocated
 * @param[out] error_code
 */
int attach_compute(const char *circuit_id, const char *afu_name,
                   iport_list *ports, const uint64_t effective_addr,
                   const uint64_t size);
/**
 * Disconnect and free allocated memory
 *
 * @param[in] circuit_id: circuit identifier
 * @param[out] error_code
 */
int detach_memory(const char *circuit_id);

/**
 * Register detachment of memory on compute side
 *
 * @param[in] circuit_id: circuit identifier
 * @param[out] error_code
 */
int detach_compute(const char *circuit_id);

#endif
