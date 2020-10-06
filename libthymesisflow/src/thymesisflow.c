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

/// @file thymesisflow.c
#include "thymesisflow.h"
#include <fcntl.h>

int unplug_memory_blocks(const uint64_t memory_size) {

    uint64_t probe_addr = INIT_PROBE_ADDR;

    char *offline_command = "offline";
    for (int i = 0; i < memory_size / MEMBLOCK_SIZE;
         i++, probe_addr += MEMBLOCK_SIZE) {
        char offline_buff[128];

        snprintf(offline_buff, sizeof(offline_buff),
                 "/sys/devices/system/memory/memory%lu/state",
                 probe_addr / MEMBLOCK_SIZE);

        // log_info("offlining - %s\n", offline_buff);

        if (access(offline_buff, W_OK) == -1) {

            log_error_ext("cannot get write access- %s\n", offline_buff);
            return -1;
        }

        FILE *fd = fopen(offline_buff, "w");
        if (fd == NULL) {
            log_error_ext("error opening - %s\n", offline_buff);
            return -1;
        }

        // printf("opened - %s\n", offline_buff);
        int res = fprintf(fd, "%s\n", offline_command);

        if (res <= 0) {
            log_error_ext("error offlining...\n");
        }
        // printf("written - %d\n", res);
        fflush(fd);
        fclose(fd);
    }
    return DETACH_OK;
}

int detach_memory(const char *circuit_id) {
    connection *conn = get_conn(circuit_id);
    if (conn == NULL) {
        log_error_ext("error fetching connection: %s", circuit_id);
        return ERR_MISSING_CID;
    }
    log_info_ext("thymesisflow - memory detach - circuit: %s\n", circuit_id);

    // free memory
    if (conn->ea == NULL) {
        log_warn_ext(
            "thymesisflow - cannot free memory for circuit -  %s - NULL "
            "ref pointer \n",
            circuit_id);
    } else {
        free(conn->ea);
    }

    // delete connection
    int res_del_code;
    // remove connection even in case of errors
    if ((res_del_code = del_conn(circuit_id)) != CONN_SUCCESS) {
        log_error_ext(
            "error registering detachment for circuit %s - error: %d\n",
            circuit_id, res_del_code);
    }
    return DETACH_OK;
}

int detach_compute(const char *circuit_id) {
    connection *conn = get_conn(circuit_id);
    if (conn == NULL) {
        log_error_ext("error fetching circuit: %s", circuit_id);
        return ERR_MISSING_CID;
    }
    log_info_ext("compute detach - circuit: %s\n", circuit_id);
    // Check return value in detach

#ifdef MOCK // assume correct detach when we mock the connection
    int res = DETACH_OK;
#else
    int res = unplug_memory_blocks(conn->size);

#endif

    int res_del_code;
    // remove connection even in case of errors
    if ((res_del_code = del_conn(circuit_id)) != CONN_SUCCESS) {
        log_error_ext(
            "thymesisflow - error registering detachment for circuit %s "
            "- error: %d\n",
            circuit_id, res_del_code);
    }

    return res;
}

int attach_memory(const char *circuit_id, const char *afu_name,
                  iport_list *ports, const uint64_t size, uint64_t *eaptr) {

    connection *conn = new_conn(circuit_id, afu_name, size);

    add_conn(conn);

#ifdef MOCK
    log_info_ext("mocking memory connection by allocating only 256 MB...\n");

    if (posix_memalign((void **)&conn->ea, CACHE_ALIGNMENT, (256 >> 20)) != 0) {
        log_error_ext("unable to allocate %ld bytes memory\n", size);
        return 1;
    }

    // set effective address reference for caller
    *eaptr = (uint64_t)conn->ea;

#else

    log_info_ext("Allocating aligned memory\n");
    if (posix_memalign((void **)&conn->ea, CACHE_ALIGNMENT, size) != 0) {
        log_error_ext("unable to allocate %ld bytes memory\n", size);
        return 1;
    }
    if (mlock(conn->ea, size) < 0) {
        // error mem lock ERR_MLOCK
        log_error_ext("cannot mlock memory for circuit - %s\n", circuit_id);
        return 1;
    }

    log_info_ext("memsetting to zero\n");
    memset(conn->ea, '\0', size);

    int open_res = 0;
    if ((open_res = setup_afu_memory(conn)) != 0) {
        return open_res;
    }

    log_info("successfully opened afu: %s\n", afu_name);

    // set effective address reference for caller
    *eaptr = (uint64_t)conn->ea;

#endif
    return ATTACH_OK;
}

int hotplug_memory_blocks(uint64_t memory_size) {

    char file_buff[128];

    uint64_t probe_addr = INIT_PROBE_ADDR;

    char *online_command = "online_movable";
    for (int i = 0; i < memory_size / MEMBLOCK_SIZE;
         i++, probe_addr += MEMBLOCK_SIZE) {

        snprintf(file_buff, sizeof(file_buff), "%s/memory%lu", MEMORY_PATH,
                 probe_addr / MEMBLOCK_SIZE);

        if (access(file_buff, F_OK) == -1) {
            // file does not exist
            FILE *fd = fopen(PROBE_PATH, "w");

            if (fd == NULL) {
                log_error_ext("error opening probe file: %s\n", PROBE_PATH);
            } else {
                int res = fprintf(fd, "%lu", probe_addr);
                if (res <= 0) {
                    log_error_ext("error probing %lu - res: %d...\n",
                                  probe_addr, res);
                }
                fflush(fd);
                fclose(fd);
            }
        }

        char online_buff[128];

        snprintf(online_buff, sizeof(online_buff),
                 "/sys/devices/system/memory/memory%lu/state",
                 probe_addr / MEMBLOCK_SIZE);

        if (access(online_buff, W_OK) == -1) {

            log_error_ext("Unable to get write access on    %s\n", online_buff);
            return ERR_PROBE;
        }

        FILE *fd = fopen(online_buff, "w");
        if (fd == NULL) {
            log_error_ext("Error opening %s\n", online_buff);
            return ERR_PROBE;
        }

        int res = fprintf(fd, "%s\n", online_command);

        if (res <= 0) {
            log_error_ext("error onlining...\n");
            return ERR_PROBE;
        }
        fflush(fd);
        fclose(fd);
    }
    return ATTACH_OK;
}

int attach_compute(const char *circuit_id, const char *afu_name,
                   iport_list *ports, const uint64_t effective_addr,
                   const uint64_t size, int no_hotplug) {

    if (ports == NULL) {
        log_error_ext("ports cannot be null\n");
        return ERR_PORT_UNSUPPORTED;
    } else if (ports->next != NULL) {
        log_warn_ext("Only single port setup supported\n");
        return ERR_PORT_UNSUPPORTED;
    }

    connection *conn = new_conn(circuit_id, afu_name, size);

    add_conn(conn);
#ifdef MOCK
    log_info_ext("mocking memory attachment on compute side\n");
    if (no_hotplug)
        log_info_ext("Request with no_hotplug flag set")
    return ATTACH_OK;
#else

    int open_res = 0;
    if ((open_res = setup_afu_compute(conn, effective_addr, ports)) != 0) {
        return open_res;
    }

    log_info_ext("AFU %s succesfully opened\n", afu_name);

    // Allow the AURORA channel to finish the setup step
    // evaluate if we can decrease this value
    sleep(5);

    if (no_hotplug){
        log_debug_ext("No need to hoplug this memory chunk");
        return ATTACH_OK;
    }

    return hotplug_memory_blocks(size); // add size to
#endif
}
