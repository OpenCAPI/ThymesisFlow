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

#include "proto.h"
#include "server.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"
#include <assert.h>
#include <signal.h>
#include <sys/stat.h>

#include <getopt.h>

void help() {
    fprintf(stderr, "Usage thymesisf-agent [options]\n\n");
    fprintf(stderr,
            "thymesisf-agent is a server responsible for configuring AFUs\n");
    fprintf(
        stderr,
        "on each node participating in a disaggregated memory connection\n\n");
    fprintf(stderr, "Options (all required):\n\n");
    fprintf(stderr,
            "    -s SOCKET_PATH\n\t\t linux socket fullpath, used by the "
            "server to receive client requests\n");
    fflush(stdout);
}

void handle_request(int cfd) {
    char *msg = read_message(cfd);
    char *response_msg;

    if (msg == NULL) {
        // TODO: return error reading message

        response_msg = new_proto_msg();
        return;
    } else {
        char *msgtype = (char *)malloc(MSGTYPE_SIZE);
        memcpy(msgtype, msg + CIRCUIT_ID_SIZE, MSGTYPE_SIZE);

        if (strncmp(msgtype, MEMORY_ATTACH, MSGTYPE_SIZE) == 0) {
            response_msg = proto_attach_memory(msg);

        } else if (strncmp(msgtype, MEMORY_DETACH, MSGTYPE_SIZE) == 0) {
            response_msg = proto_detach_memory(msg);

        } else if (strncmp(msgtype, COMPUTE_ATTACH, MSGTYPE_SIZE) == 0) {
            response_msg = proto_attach_compute(msg);

        } else if (strncmp(msgtype, COMPUTE_DETACH, MSGTYPE_SIZE) == 0) {
            response_msg = proto_detach_compute(msg);

        } else {
            log_warn("unknown mode %s\n", msgtype);
            response_msg = set_unknown_mode_response(msg);
            // fill error response
        }
        free(msgtype);
    }

    write_message(cfd, response_msg);
    // error checking

    if (msg != NULL)
        free(msg);
    if (response_msg != NULL)
        free(response_msg);
}

int main(int argc, char *argv[]) {

    char *sock_path = NULL;

    struct sockaddr_un addr;
    int sfd, cfd;

    int opt;

    while ((opt = getopt(argc, argv, ":s")) != -1) {
        switch (opt) {
        case 's':
            sock_path = optarg;
            break;
        case '?':
            help();
            exit(EXIT_FAILURE);
        default:
            help();
            exit(EXIT_FAILURE);
        }
    }

    if (sock_path == NULL) {
        sock_path = SOCK_PATH;
    }
    log_info("using socket: %s\n", sock_path);

    log_info("starting server with sock_path: %s\n", sock_path);

    // avoid exiting when reading on closed pipe, need better handling
    signal(SIGPIPE, SIG_IGN);

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        log_error("error creating socket: %s\n", strerror(errno));
        return 1;
    }

    if (remove(sock_path) == -1 && errno != ENOENT) {
        log_info("error removing %s.. \n", sock_path);
        return 2;
    }

    memset(&addr, 0, sizeof(addr.sun_path) - 1);

    addr.sun_family = AF_UNIX; // use unix socket
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        log_error("error while binding the socket\n");
        return 1;
    }

    if (listen(sfd, BACKLOG) == -1) {
        log_error("error during listen\n");
    }

    log_info("Starting thymesisflow server...\n");

    for (;;) {
        log_info("Ready to accept new requests...\n");

        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            log_info("error during accept\n");
            return 1;
        }
        handle_request(cfd);

        log_info("closing connection...\n");

        if (close(cfd) == -1) {
            log_error("error during close\n");
        }
    }
}
