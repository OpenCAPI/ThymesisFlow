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

/// @file proto.c
#include "proto.h"

/**
 * Print serialized message
 *
 * @param[in] msg: serialized proto message
 */
void print_proto_msg(char *msg) {
    for (size_t i = 0; i < MSG_SIZE; i++) {
        // log_info("%c",msg[i]);
        printf("%02x", msg[i]);
    }
    log_info("\n");
}

/**
 * Disconnect and free allocated memory
 *
 * @param[in] msg: serialized proto message
 * @param[out] iport_list: list of AFU ports to be used in the connection
 */
iport_list *get_iports(const char *msg) {
    uint8_t ports = 0;
    memcpy(&ports, msg + IPORT_OFFSET, IPORT_SIZE);
    // log_info("ports hex representation: %x\n", ports);
    return unmarshal_iports(ports);
}

/**
 * Set ports in a serialized message
 *
 * @param[in] msg: serialized proto message
 * @param[out] iport_list: list of AFU ports to be used in the connection
 */
void set_iports(char *msg, const iport_list *ports) {
    uint8_t port_rep = marshal_iports(ports);
    // log_info("setting ports hex rep: %x\n", port_rep);
    memcpy(msg + IPORT_OFFSET, &port_rep, IPORT_SIZE);
}

/**
 * Extract circuit from a serialized message
 *
 * @param[in] msg: serialized proto message
 * @param[in] circuit_id: pointer that will reference a new dynamically
 * allocated array containing the circuit id (remember to free)
 * @param[out] result: 0 for success, -1 in case of error
 */
int get_circuitid(const char *msg, char *circuit_id) {
    // circuit_id needs to be initialized
    if (msg == NULL || circuit_id == NULL) {
        return -1;
    }
    memcpy(circuit_id, msg + CIRCUIT_ID_OFFSET, CIRCUIT_ID_SIZE);
    circuit_id[CIRCUIT_ID_SIZE] = '\0';
    return 0;
}

/**
 * Set circuit id in a message
 *
 * @param[in] msg: serialized proto message
 * @param[in] circtui_id: circuit uuid
 * @param[out] int: status code, 0 if the circuit_id is stored correctly
 */
int set_circuitid(char *msg, const char *circuit_id) {
    if (circuit_id == NULL) {
        return ERR_NULL_PARAM;
    }
    if (msg == NULL) {
        return ERR_MSGNULL;
    }
    memcpy(msg + CIRCUIT_ID_OFFSET, circuit_id, CIRCUIT_ID_SIZE);
    return 0;
}

/**
 * Extract message type pointer from a serialized message
 *
 * @param[in] msg: serialized proto message
 * @param[out] ptr: offset in the message where the circuit_id is stored
 */
int get_msgtype(const char *msg, char *msgtype) {
    if (msg == NULL || msgtype == NULL) {
        return -1;
    }
    memcpy(msgtype, msg + MSGTYPE_OFFSET, MSGTYPE_SIZE);
    msgtype[MSGTYPE_SIZE] = '\0';
    return 0;
}

/**
 * Set message type in a message
 *
 * @param[in] msg: serialized proto message
 * @param[in] msgtype: message type #ATTACH_COMPUTE, #ATTACH_MEMORY,
 * #DETACH_COMPUTE, #DETACH_MEMORY
 * @param[out] int: status code, 0 if the circuit_id is stored correctly
 */
int set_msgtype(char *msg, const char *msgtype) {

    if (msgtype == NULL) {
        return ERR_NULL_PARAM;
    }

    if (msg == NULL) {
        return ERR_MSGNULL;
    }
    if (strcmp(msgtype, COMPUTE_ATTACH) != 0 &&
        strcmp(msgtype, COMPUTE_DETACH) != 0 &&
        strcmp(msgtype, MEMORY_ATTACH) != 0 &&
        strcmp(msgtype, MEMORY_DETACH) != 0 &&
        strcmp(msgtype, CRESPONSE_ATTACH) != 0 &&
        strcmp(msgtype, CRESPONSE_DETACH) != 0 &&
        strcmp(msgtype, MRESPONSE_ATTACH) != 0 &&
        strcmp(msgtype, MRESPONSE_DETACH) != 0) {
        return ERR_MSGTYPE;
    }
    memcpy(msg + MSGTYPE_OFFSET, msgtype, MSGTYPE_SIZE);
    return 0;
}

int get_afu_name(const char *msg, char *afu_name) {
    if (msg == NULL || afu_name == NULL) {
        return -1;
    }
    memcpy(afu_name, msg + AFUNAME_OFFSET, AFUNAME_SIZE);
    afu_name[AFUNAME_SIZE] = '\0';
    return 0;
}

int set_afu(char *msg, const char *afu) {
    if (afu == NULL)
        return ERR_NULL_PARAM;

    if (strlen(afu) == 0)
        return ERR_EMPTY_PARAM;

    if (msg == NULL)
        return ERR_MSGNULL;

    memcpy(msg + AFUNAME_OFFSET, afu, AFUNAME_SIZE);
    return 0;
}

// error if size <=0
uint64_t get_size(const char *msg) {
    if (msg == NULL)
        return ERR_MSGNULL;

    uint64_t size = 0;
    memcpy(&size, msg + MEM_SIZE_OFFSET, MEM_SIZE);
    return size;
}

int set_size(char *msg, uint64_t size) {
    if (msg == NULL)
        return ERR_MSGNULL;
    if (size <= 0)
        return ERR_INT_PARAM;
    if (size % MEMBLOCK_SIZE != 0)
        return ERR_MEMBLOCK_SIZE;

    memcpy(msg + MEM_SIZE_OFFSET, &size, MEM_SIZE);
    return 0;
}

// return effective address
uint64_t get_ea(const char *msg) {
    if (msg == NULL)
        return ERR_MSGNULL;

    uint64_t effective_addr = 0;
    memcpy(&effective_addr, msg + EA_OFFSET, EA_SIZE);
    return effective_addr;
}

int set_ea(char *msg, uint64_t ea) {
    if (msg == NULL)
        return ERR_MSGNULL;
    if (ea == 0)
        return ERR_INT_PARAM;

    memcpy(msg + EA_OFFSET, &ea, EA_SIZE);
    return 0;
}

int set_no_hotplug(char *msg, int no_hotplug) {
    if (msg == NULL)
        return ERR_MSGNULL;
    if (no_hotplug > 1 || no_hotplug < 0)
        return ERR_INT_PARAM;

    memcpy(msg + NO_HOTPLUG_OFFSET, &no_hotplug, NO_HOTPLUG_SIZE);
    return 0;
}

int get_no_hotplug(const char *msg) {
    int no_hotplug = 0;
    memcpy(&no_hotplug, msg + NO_HOTPLUG_OFFSET, NO_HOTPLUG_SIZE);
    return no_hotplug;
}

void set_status(char *msg, int status) {

    log_info("setting status: %d\n", status);
    memcpy(msg + ERROR_OFFSET, &status, ERROR_SIZE);
}

int get_status(const char *msg) {
    int status = 0;
    memcpy(&status, msg + ERROR_OFFSET, ERROR_SIZE);
    return status;
}

char *new_circuitid() {
    return (char *)malloc(sizeof(char) * CIRCUIT_ID_SIZE + 1);
}

char *new_afu_name() { return (char *)malloc(sizeof(char) * AFUNAME_SIZE + 1); }

char *new_msgtype() { return (char *)malloc(sizeof(char) * MSGTYPE_SIZE + 1); }

char *set_unknown_mode_response(char *msg) {
    char *response_msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    char *circuit_id = (char *)malloc(sizeof(char) * CIRCUIT_ID_SIZE + 1);
    if (get_circuitid(msg, circuit_id) != 0) {
        log_warn("error fetching circuit from message\n");
    }

    if (set_circuitid(response_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");

    char *afu_name = new_afu_name();
    if (get_afu_name(msg, afu_name) != 0) {
        log_warn("error fetching afu_name\n");
    }
    set_afu(response_msg, afu_name);
    free(circuit_id);
    free(afu_name);
    return response_msg;
}

char *proto_attach_memory(const char *msg) {
    log_info("allocating memory side...\n");

    char *circuit_id = new_circuitid();
    if (get_circuitid(msg, circuit_id) != 0) {
        log_warn("error fetching circuit from message\n");
    }

    char *afu_name = new_afu_name();
    if (get_afu_name(msg, afu_name) != 0) {
        log_warn("error fetching afu_name\n");
    }
    iport_list *ports = get_iports(msg);

    uint64_t size = get_size(msg);

    log_info("circuit: %s - allocating %lu bytes using afu: %s...\n",
             circuit_id, size, afu_name);

    uint64_t effective_address;

    int res =
        attach_memory(circuit_id, afu_name, ports, size, &effective_address);
    free_iport_list(ports);

    // CHECK ERROR

    log_info("returing effective address: %lx\n", effective_address);

    char *response_msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    if (set_circuitid(response_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");

    if (set_msgtype(response_msg, MRESPONSE_ATTACH) < 0) {
        log_warn("error setting message type \n");
    }
    set_afu(response_msg, afu_name);
    set_iports(response_msg, ports);
    set_size(response_msg, size);
    set_ea(response_msg, effective_address);
    set_status(response_msg, res);

    free(circuit_id);
    free(afu_name);
    return response_msg;
}

char *proto_attach_compute(const char *msg) {

    log_info("allocating compute side...\n");

    char *circuit_id = new_circuitid();
    if (get_circuitid(msg, circuit_id) != 0) {
        log_warn("error fetching circuit from message\n");
    }

    char *afu_name = new_afu_name();
    if (get_afu_name(msg, afu_name) != 0) {
        log_warn("error fetching afu_name\n");
    }

    iport_list *ports = get_iports(msg);

    uint64_t size = get_size(msg);

    uint64_t ea = get_ea(msg);

    int no_hotplug = get_no_hotplug(msg);

    // uint64_t memsize = get_size(msg);

    log_info("requested compute mode - size %lu - effective address: %lu \n",
             size, ea);

    int err = attach_compute(circuit_id, afu_name, ports, ea, size, no_hotplug);
    free_iport_list(ports);
    // error check
    char *response_msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    if (set_circuitid(response_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");

    if (set_msgtype(response_msg, CRESPONSE_ATTACH) < 0) {
        log_warn("error setting message type \n");
    }
    set_size(response_msg, size);
    set_afu(response_msg, afu_name);
    set_iports(response_msg, ports);
    set_status(response_msg, err);

    free(circuit_id);
    free(afu_name);
    return response_msg;
}

char *proto_detach_compute(const char *msg) {
    char *response_msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    char *circuit_id = new_circuitid();
    if (get_circuitid(msg, circuit_id) != 0) {
        log_warn("error fetching circuit from message\n");
    }

    log_info("deleting compute connection- cid: %s \n", circuit_id);

    if (set_circuitid(response_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");

    int err = detach_compute(circuit_id);
    if (set_msgtype(response_msg, CRESPONSE_DETACH) < 0) {
        log_warn("error setting message type \n");
    }
    set_status(response_msg, err);

    free(circuit_id);

    return response_msg;
}

char *proto_detach_memory(const char *msg) {
    if (msg == NULL) {
        log_warn("message should not be NULL\n");
    }
    char *response_msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    char *circuit_id = new_circuitid();
    if (get_circuitid(msg, circuit_id) != 0) {
        log_warn("error fetching circuit from message\n");
    }

    log_info("deleting memory connection - cid: %s \n", circuit_id);

    if (set_circuitid(response_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");

    if (set_msgtype(response_msg, MRESPONSE_DETACH) < 0) {
        log_warn("error setting message type \n");
    }
    int err = detach_memory(circuit_id);
    set_status(response_msg, err);
    free(circuit_id);

    return response_msg;
}

char *new_proto_msg() {
    char *response_msg = (char *)malloc(sizeof(char) * MSG_SIZE);
    // fill with error message
    return response_msg;
}

char *marshal_detach_memory_request(const char *circuit_id) {
    char *request_msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    if (set_circuitid(request_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");

    if (set_msgtype(request_msg, MEMORY_DETACH) < 0) {
        log_warn("error setting message type \n");
    }

    return request_msg;
}

char *marshal_detach_compute_request(const char *circuit_id) {
    char *request_msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    if (set_circuitid(request_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");

    if (set_msgtype(request_msg, COMPUTE_DETACH) < 0) {
        log_warn("error setting message type \n");
    }

    return request_msg;
}

char *marshal_attach_memory_request(const char *circuit_id, const char *afu,
                                    const iport_list *ports,
                                    const uint64_t memsize) {
    char *request_msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    if (set_circuitid(request_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");

    if (set_msgtype(request_msg, MEMORY_ATTACH) < 0) {
        log_warn("error setting message type \n");
    }
    set_afu(request_msg, afu);
    set_iports(request_msg, ports);
    set_size(request_msg, memsize);
    return request_msg;
}

char *marshal_attach_compute_request(const char *circuit_id, const char *afu,
                                     const iport_list *ports,
                                     const uint64_t memsize,
                                     const uint64_t ea,
				     int no_hotplug) {
    char *request_msg = (char *)malloc(sizeof(char) * MSG_SIZE);
    if (set_circuitid(request_msg, circuit_id) < 0)
        log_warn("error setting message circuit id\n");
    if (set_msgtype(request_msg, COMPUTE_ATTACH) < 0) {
        log_warn("error setting message type \n");
    }
    set_afu(request_msg, afu);
    set_iports(request_msg, ports);
    set_size(request_msg, memsize);
    set_ea(request_msg, ea);
    set_no_hotplug(request_msg, no_hotplug);
    return request_msg;
}

void unmarshal_response(char *msg, pmessage *rsp) {

    char *circuit_id = new_circuitid();
    if (get_circuitid(msg, circuit_id) != 0) {
        log_warn("error fetching circuit from message\n");
    }
    strncpy(rsp->circuitid, circuit_id, CIRCUIT_ID_SIZE + 1);
    // log_info("circuitid: %s\n", rsp->circuitid);

    char *afu_name = new_afu_name();
    if (get_afu_name(msg, afu_name) != 0) {
        log_warn("error fetching afu_name\n");
    }
    strncpy(rsp->afu, afu_name, AFUNAME_SIZE + 1);
    // log_info("afu_name: %s\n", rsp->afu);

    char *msgtype = new_msgtype();
    if (get_msgtype(msg, msgtype) != 0) {
        log_warn("error fetching msgtype\n");
    }
    strncpy(rsp->msgtype, msgtype, MSGTYPE_SIZE + 1);
    // log_info("msgtype: %s\n", rsp->circuitid);

    rsp->size = get_size(msg);
    rsp->ea = get_ea(msg);
    rsp->status = get_status(msg);
    rsp->no_hotplug = get_hotplug(msg);
    free(afu_name);
    free(circuit_id);
}

char *read_message(int fd) {
    char *msg = (char *)malloc(sizeof(char) * MSG_SIZE);

    for (size_t i = 0; i < MSG_SIZE; i++) {
        int num_read = recv(fd, msg + i, MSG_SIZE, 0);
        i += num_read;
    }

    return msg;
}

int read_pmessage(int fd, pmessage *resp) {

    char *resp_msg = read_message(fd);

    if (resp_msg == NULL) {
        return -1;
    }
    unmarshal_response(resp_msg, resp);

    return 0;
}

int write_message(int fd, const char *msg) {
    for (size_t i = 0; i < MSG_SIZE; i++) {
        int num_read = write(fd, msg, MSG_SIZE);
        i += num_read;
    }

    return 0;
}

int open_client_connection(const char *sock_path) {
    if (sock_path == NULL || 0 == strlen(sock_path))
        return -1;

    struct sockaddr_un addr;
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        log_info("error creating socket\n");
        return -1;
    }
    memset(&addr, 0, sizeof(addr.sun_path) - 1);

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) ==
        -1)
        return -1;
    return sfd;
}
