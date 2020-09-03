/// @file proto.h
#if !defined(THYMESISFLOW_PROTO_H)
#define THYMESISFLOW_PROTO_H

#include "errcode.h"
#include "iport.h"
#include "logger.h"
#include "protoconst.h"
#include "thymesisflow.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef struct proto_response {
    char circuitid[CIRCUIT_ID_SIZE + 1];
    char msgtype[MSGTYPE_SIZE + 1];
    char afu[AFUNAME_SIZE + 1];
    int status;
    uint64_t size;
    uint64_t ea;
} pmessage;

/**
 * Open linux socket connection towards the server
 *
 * @param[in] circuit_id: circuit identifier
 * @param[out] sfd: socket file descriptor if success, -1 if failure
 */
int open_client_connection(const char *sock_path);

/**
 * Print the content of the a message sent on the socket. Used for DEBUG
 * purposes
 *
 * @param[in] msg: message sent through the socket
 */
void print_proto_msg(char *msg);

/**
 * Print the content of the a message sent on the socket. Used for DEBUG
 * purposes
 *
 * @param[in] msg: message sent through the socket
 * @param[in] rsp: pointer to data struct that will contain the unmarshalled
 * response
 */
void unmarshal_response(char *msg, pmessage *rsp);

/**
 * Read message from from the socket connection and unmarshal the content
 *
 * @param[in] cfd: socket connection
 * @param[in] rsp: pointer to data struct that will contain the unmarshalled
 * response
 */
int read_pmessage(int cfd, pmessage *rsp);

/**
 * Read message from the socket connection
 *
 * @param[in] cfd: socket connection
 * @param[in] msg: array containing the marshalled response
 */
char *read_message(int cfd);

/**
 * Write message on a socket connection
 *
 * @param[in] fd: socket connection
 * @param[in] msg: array containing the marshalled response
 * @param[out] result: 0 if success, -1 if error in writing the message
 */
int write_message(int fd, const char *msg);

/**
 * Marshal request to setup new thymesisflow on memory-stealing node
 *
 * @param[in] circuitid: circuit uuid
 * @param[in] afu: name of the Accelerated Function Unit
 * @param[in] ports: AFU port to be used
 * @param[in] size: memory allocation size (in bytes)
 * @param[out] msg: array containing the marshalled request
 */
char *marshal_attach_memory_request(const char *circuitid, const char *afu,
                                    const iport_list *ports,
                                    const uint64_t size);

/**
 * Marshal request to setup new thymesisflow on memory-stealing node
 *
 * @param[in] circuitid: circuit uuid
 * @param[in] afu: name of the Accelerated Function Unit
 * @param[in] ports: AFU port to be used
 * @param[in] size: memory allocation size (in bytes)
 * @param[in] ea: effective address used for memory access translation
 * @param[out] msg: array containing the marshalled request
 */
char *marshal_attach_compute_request(const char *circuitid, const char *afu,
                                     const iport_list *ports,
                                     const uint64_t memsize, const uint64_t ea);

/**
 * Marshal request to tear down a thymesisflow on memory-stealing node
 *
 * @param[in] circuitid: circuit uuid associate to the connection to tear down
 * @param[out] msg: array containing the marshalled request
 */
char *marshal_detach_memory_request(const char *circuitid);

/**
 * Marshal request to tear down a thymesisflow on comute node
 *
 * @param[in] circuitid: circuit uuid associate to the connection to tear down
 * @param[out] msg: array containing the marshalled request
 */
char *marshal_detach_compute_request(const char *circuitid);

/**
 * Perform the steps required to setup a new thymesisflow on the compute node
 *
 * @param[in] msg: array containing the marshalled request
 * @param[out] msg: array containing the marshalled response
 */
char *proto_attach_compute(const char *msg);

/**
 * Perform the steps required to setup a new thymesisflow on the memory-stealing
 * node
 *
 * @param[in] msg: array containing the marshalled request
 * @param[out] msg: array containing the marshalled response
 */
char *proto_attach_memory(const char *msg);

/**
 * Perform the steps required to tear down a thymesisflow on the memory-stealing
 * node
 *
 * @param[in] msg: array containing the marshalled request
 * @param[out] msg: array containing the marshalled response
 */
char *proto_detach_memory(const char *msg);

/**
 * Perform the steps required to setup a new thymesisflow on the compute node
 *
 * @param[in] msg: array containing the marshalled request
 * @param[out] msg: array containing the marshalled response
 */
char *proto_detach_compute(const char *msg);

/**
 * Return error message if the message type is not recognized
 *
 * @param[in] msg: array containing the marshalled request
 * @param[out] msg: array containing the marshalled response
 */
char *set_unknown_mode_response(char *msg);

/**
 * Dynamically allocate memory for a protocol message
 *
 * @param[out] ptr: return a pointer to newly allocated memory
 */
char *new_proto_msg();

#endif
