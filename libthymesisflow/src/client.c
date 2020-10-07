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

/// @file client.c
#include "client.h"

pmessage send_cmd(const char *msg, const char *sock_path) {
    int sfd = open_client_connection(sock_path);
    if (write_message(sfd, msg) != 0) {
        log_warn("error sending message\n");
    }

    pmessage rsp;
    read_pmessage(sfd, &rsp);

    if (close(sfd) == -1) {
        log_error("error while closing socket file descriptor\n");
    }
    return rsp;
}

pmessage send_detach_memory_msg(const char *circuitid, const char *sock_path) {
    char *msg = marshal_detach_memory_request(circuitid);

    pmessage rsp = send_cmd(msg, sock_path);

    log_debug("circuitid: %s - type: %s - status: %d\n", rsp.circuitid,
              rsp.msgtype, rsp.status);

    free(msg);
    return rsp;
}

pmessage send_detach_compute_msg(const char *circuitid, const char *sock_path) {
    char *msg = marshal_detach_compute_request(circuitid);

    pmessage rsp = send_cmd(msg, sock_path);

    log_debug("circuitid: %s - type: %s - status: %d\n", rsp.circuitid,
              rsp.msgtype, rsp.status);

    free(msg);
    return rsp;
}

pmessage send_attach_memory_msg(const char *circuitid, const char *afu,
                                const iport_list *ports, const uint64_t size,
                                const char *sock_path) {

    char *msg = marshal_attach_memory_request(circuitid, afu, ports, size);

    pmessage rsp = send_cmd(msg, sock_path);

    log_debug("circuitid: %s - type: %s - afu %s - size %lu  - ea(hex): %lx - "
              "ea(int):%lu - status: %d\n",
              rsp.circuitid, rsp.msgtype, rsp.afu, rsp.size, rsp.ea, rsp.ea,
              rsp.status);

    free(msg);
    return rsp;
}

pmessage send_attach_compute_msg(const char *circuitid, const char *afu,
                                 const iport_list *ports, const uint64_t size,
                                 const uint64_t ea, int no_hotplug, const char *sock_path) {

    char *msg = marshal_attach_compute_request(circuitid, afu, ports, size, ea, no_hotplug);

    pmessage rsp = send_cmd(msg, sock_path);

    log_debug("circuitid: %s - type: %s - afu %s - size %lu  - status: %d\n",
              rsp.circuitid, rsp.msgtype, rsp.afu, rsp.size, rsp.status);

    free(msg);
    return rsp;
}
