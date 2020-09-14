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
#include "agent.h"

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



int main(int argc, char *argv[]) {

    char *sock_path = NULL;


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
    run_agent(sock_path);

}
