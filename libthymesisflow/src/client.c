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
                                 const uint64_t ea, const char *sock_path) {

    char *msg = marshal_attach_compute_request(circuitid, afu, ports, size, ea);

    pmessage rsp = send_cmd(msg, sock_path);

    log_debug("circuitid: %s - type: %s - afu %s - size %lu  - status: %d\n",
              rsp.circuitid, rsp.msgtype, rsp.afu, rsp.size, rsp.status);

    free(msg);
    return rsp;
}
