/// @file client.h
#if !defined(THYMESISFLOW_CLIENT_H)
#define THYMESISFLOW_CLIENT_H

#include "errcode.h"
#include "iport.h"
#include "logger.h"
#include "proto.h"

// fixed afu port in current design
#define AFU_PORT 2

/**
 *  Send setup request to the memory node daemon through the linux socket
 *
 * @param[in] circuit_id: circuit identifier
 * @param[in] afu: AFU name
 * @param[in] iport_list: list of ports to be used for the attachemnt (Currently
 * single port supported)
 * @param[in] size: size of memory to be allocated
 * @param[in] sock_path: path to the linux socket
 * @param[out] pmessage unmarshalled response from the server
 */
pmessage send_attach_memory_msg(const char *circuitid, const char *afu,
                                const iport_list *ports, const uint64_t size,
                                const char *sock_path);

/**
 *  Send setup request to the compute node daemon through the linux socket
 *
 * @param[in] circuit_id: circuit identifier
 * @param[in] afu: AFU name
 * @param[in] iport_list: list of ports to be used for the attachemnt (Currently
 * single port supported)
 * @param[in] size: size of memory to be allocated
 * @param[in] ea: effective address to be passed to the AFU during the setup
 * @param[in] sock_path: path to the linux socket
 * @param[out] pmessage unmarshalled response from the server
 */
pmessage send_attach_compute_msg(const char *circuitid, const char *afu,
                                 const iport_list *ports, const uint64_t size,
                                 const uint64_t ea, const char *sock_path);

/**
 *  Send detachment request to the memory node daemon through the linux socket
 *
 * @param[in] circuit_id: circuit identifier
 * @param[in] sock_path: path to the linux socket
 * @param[out] pmessage unmarshalled response from the server
 */
pmessage send_detach_memory_msg(const char *circuitid, const char *sock_path);

/**
 *  Send detachment request to the compute node daemon through the linux socket
 *
 * @param[in] circuit_id: circuit identifier
 * @param[in] sock_path: path to the linux socket
 * @param[out] pmessage unmarshalled response from the server
 */
pmessage send_detach_compute_msg(const char *circuitid, const char *sock_path);

#endif
