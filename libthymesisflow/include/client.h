/*
 * Copyright 2019 International Business Machines
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
 */

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
 * @param[in] no_hotplug: do not hotploug memory
 * @param[in] sock_path: path to the linux socket
 * @param[out] pmessage unmarshalled response from the server
 */
pmessage send_attach_compute_msg(const char *circuitid, const char *afu,
                                 const iport_list *ports, const uint64_t size,
                                 const uint64_t ea, int no_hotplug, const char *sock_path);

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
