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

#include "client.h"
#include <getopt.h>

void helper_memory_attach() {
    fprintf(stderr, "Usage thymesisf-cli attach-memory [options]\n\n");
    fprintf(stderr, "Options (all required):\n\n");
    fprintf(stderr, "    --afu AFU_NAME \n\t\tName of the Accelerated Function "
                    "Unit (example: IBM,RMEM\n");
    fprintf(stderr, "    --cid CIRCUIT_UUID  \n\t\tCircuit Identifier\n");
    fprintf(stderr,
            "    --size MEMORY_SIZE\n\t\t in bytes (bigger than zero) and "
            "multiple of MEMBLOCK_SIZE (%d)\n",
            MEMBLOCK_SIZE);
    // fprintf(stderr,"  --port PORT_NUMBER\n\t\t AFU port to be used (example:
    // 1 or 2)\n");
}

int valid_mem_size(uint64_t size) { return (size % MEMBLOCK_SIZE == 0); }

int handle_memory_attach(const char *cid, const char *afu, const iport_list *pl,
                         const uint64_t size, const char *sock_path) {
    if (cid == 0 || strcmp(cid, "") == 0 || size == 0 || pl == NULL ||
        strcmp(afu, "") == 0) {
        helper_memory_attach();
        return EXIT_FAILURE;
    }

    if (!valid_mem_size(size)) {
        log_info(
            "Memory allocation needs to be a multiple of MEMBLOCK_SIZE (%d)\n",
            MEMBLOCK_SIZE);
        helper_memory_attach();
        return EXIT_FAILURE;
    }

    log_info("Attaching memory - cid: %s afu: %s:%d - size: %ld\n", cid, afu,
             pl->nport, size);

    pmessage mresp = send_attach_memory_msg(cid, afu, pl, size, sock_path);

    if (mresp.status == 100) {
        log_info("Successfully allocated memory - EA: 0x%lx\n", mresp.ea);
        return 0;
    } else {
        log_error("error allocating memory - status: %d\n", mresp.status);
        return 1;
    }
}

void helper_compute_attach() {
    fprintf(stderr, "Usage thymesisf-cli attach-compute [options]\n\n");
    fprintf(stderr, "Options (all required):\n\n");
    fprintf(stderr, "    --afu AFU_NAME \n\t\tName of the Accelerated Function "
                    "Unit (example: IBM,RMEM\n");
    fprintf(stderr, "    --cid CIRCUIT_UUID  \n\t\tCircuit Identifier\n");
    fprintf(stderr,
            "    --size MEMORY_SIZE\n\t\t in bytes (bigger than zero)\n");
    // fprintf(stderr,"  --port PORT_NUMBER\n\t\t AFU port to be used (example:
    // 1 or 2)\n");
    fprintf(stderr,
            "    --ea EFFECTIVE_ADDRESS\n\t\t effective address printed "
            "by the lender\n");
    fprintf(stderr, "    --no_hotplug\n\t\t attach memory without hotplugging to Linux\n");

}

int handle_compute_attach(const char *cid, const char *afu,
                          const iport_list *pl, const uint64_t size,
                          const u_int64_t ea, const char *sock_path, int no_hotplug) {
    if (cid == NULL || strlen(cid) == 0 || size == 0 || pl == NULL ||
        afu == NULL || strlen(afu) == 0 || ea == 0) {
        helper_compute_attach();
        return EXIT_FAILURE;
    }
    if (!valid_mem_size(size)) {
        log_info(
            "Memory allocation needs to be a multiple of MEMBLOCK_SIZE (%u)\n",
            MEMBLOCK_SIZE);
        helper_memory_attach();
        return EXIT_FAILURE;
    }

    log_info("attaching compute - size: %lu - using effective address: %lu\n",
             size, ea);

    pmessage cresp = send_attach_compute_msg(cid, afu, pl, size, ea, no_hotplug, sock_path);

    if (cresp.status == 100) {
        log_info("Successfully attached compute connection\n");
        return 0;
    } else {
        log_info("error allocating memory : %d\n", cresp.status);
        return 1;
    }
}

void helper_memory_detach() {
    fprintf(stderr, "Usage thymesisf-cli detach-memory [options]\n\n");
    fprintf(stderr, "Options (all required):\n\n");
    fprintf(stderr, "    --cid CIRCUIT_UUID  \n\t\tCircuit Identifier\n");
}

int handle_memory_detach(const char *cid, const char *sock_path) {
    if (cid == NULL || strcmp(cid, "") == 0) {
        helper_memory_detach();
        return 1;
    }

    log_info("Detaching memory - cid: %s\n", cid);
    pmessage mresp = send_detach_memory_msg(cid, sock_path);

    if (mresp.status == 200) {
        log_info("Successfully detached memory connection\n");
        return 0;
    } else {
        log_error("Error detaching memory connection - status: %d\n",
                  mresp.status);
        return 1;
    }
}

void helper_compute_detach() {
    fprintf(stderr, "Usage thymesisf-cli detach-compute [options]\n\n");
    fprintf(stderr, "Options (all required):\n\n");
    fprintf(stderr, "    --cid CIRCUIT_UUID  \n\t\tCircuit Identifier\n");
}

int handle_compute_detach(const char *cid, const char *sock_path) {
    if (cid == NULL || strcmp(cid, "") == 0) {
        helper_compute_detach();
        return 1;
    }
    pmessage mresp = send_detach_compute_msg(cid, sock_path);

    if (mresp.status == 200) {
        log_info("Successfully detached compute connection\n");
        return 0;
    } else {
        log_error("Error detaching compute connection - status: %d\n",
                  mresp.status);
        return 1;
    }
}

void common_helper() {
    fprintf(stderr, "Usage thymesisf-cli command [options]\n\n");
    fprintf(stderr, "thymesisf-cli is a command line interface to attach and "
                    "detach disaggregated memory\n\n");
    fprintf(stdout, "Most used commands:\n");
    fprintf(stdout, "    attach-memory - allocate memory on lender machine\n");
    fprintf(stdout,
            "    attach-compute - attach memory on the borrower machine \n");
    fprintf(stdout, "    detach-memory - free memory on lender machine\n");
    fprintf(stdout,
            "    detach-compute - detach memory on the borrower machine\n");
}

void help(const char *command) {

    if (strncmp(command, "attach-memory", MSGTYPE_SIZE))
        helper_memory_attach();
    else if (0 == strncmp(command, "detach-memory", MSGTYPE_SIZE))
        helper_memory_detach();
    else if (0 == strncmp(command, "attach-compute", MSGTYPE_SIZE))
        helper_compute_attach();
    else if (0 == strncmp(command, "detach-compute", MSGTYPE_SIZE))
        helper_compute_detach();
    else
        common_helper();
}

int main(int argc, char **argv) {

    if (argc < 2) {
        common_helper();
        exit(1);
    }

    const char *command = argv[1];

    char *cid = NULL;
    char *afu = NULL;
    uint64_t size = 0;
    uint64_t ea = 0;
    char *sock_path = NULL;
    iport_list *pl = NULL;
    static int no_hotplug;

    static struct option long_options[] = {
        {"size", required_argument, 0, 's'},
        {"afu", required_argument, 0, 'a'},
        {"ea", required_argument, 0, 'e'},
        {"cid", required_argument, 0, 'c'},
        {"no-hotplug", no_argument, &no_hotplug, 1},
        //{"port",required_argument,0,'p'},
    };

    while (1) {
        int option_index = 0;

        int c =
            getopt_long(argc, argv, "s:a:e:c:p:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 's':
            size = strtoull(optarg, NULL, 0);
            break;
        case 'a':
            // afu= (char*)malloc(AFUNAME_SIZE);
            // strcpy(afu , optarg);
            afu = optarg;
            break;
        case 'e':
            ea = strtoull(optarg, NULL, 16);
            break;
        case 'c':
            cid = optarg;
            break;
        }
    }

    // Fixed port 2
    pl = add_port(pl, AFU_PORT);

    if (sock_path == NULL) {
        sock_path = SOCK_PATH;
    }

    if (strcmp("attach-memory", command) == 0) {
        return handle_memory_attach(cid, afu, pl, size, sock_path);
    } else if (strcmp("attach-compute", command) == 0) {
        return handle_compute_attach(cid, afu, pl, size, ea, sock_path, no_hotplug);
    } else if (strcmp("detach-memory", command) == 0) {
        return handle_memory_detach(cid, sock_path);
    } else if (strcmp("detach-compute", command) == 0) {
        return handle_compute_detach(cid, sock_path);
    } else {
        common_helper();
    }
}
