#include "agent.h"

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

void graceful_termination(){
  log_info("Graceful agent termination...\n");
  exit(0);
}


void run_agent(const char* sock_path){

    struct sockaddr_un addr;
    int sfd, cfd;

    if (sock_path == NULL) {
        sock_path = SOCK_PATH;
    }
    log_info("using socket: %s\n", sock_path);

    log_info("starting server with sock_path: %s\n", sock_path);

    // avoid exiting when reading on closed pipe, need better handling
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, graceful_termination);

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        log_error("error creating socket: %s\n", strerror(errno));
        exit(1);
    }

    if (remove(sock_path) == -1 && errno != ENOENT) {
        log_info("error removing %s.. \n", sock_path);
        exit(2);
    }

    memset(&addr, 0, sizeof(addr.sun_path) - 1);

    addr.sun_family = AF_UNIX; // use unix socket
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        log_error("error while binding the socket\n");
        exit(3);
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
            exit(4);
        }
        handle_request(cfd);

        log_info("closing connection...\n");

        if (close(cfd) == -1) {
            log_error("error during close\n");
        }
    }
}
