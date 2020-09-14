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

/// @file connection.c
#include "connection.h"

uint64_t calculate_seg_offset(const uint64_t ea_base, const uint64_t seg_offset,
                              const uint64_t netid) {
    uint64_t offset;

    offset = ea_base - seg_offset;

    return (offset ^ netid);
}

connection *new_conn(const char *circuit_id, const char *afu_name,
                     const uint64_t size) {
    connection *conn = (connection *)malloc(sizeof(connection));
    memcpy(conn->circuit_id, circuit_id, CIRCUIT_ID_SIZE);
    strncpy(conn->afu_name, afu_name, AFUNAME_SIZE);
    conn->size = size;
    conn->next = NULL;
#ifndef MOCK
    conn->stat_tid = NULL;
#endif
    return conn;
}

int free_conn(connection *conn) {
    log_info_ext("freeing connection %s\n", conn->circuit_id);
    if (conn == NULL) {
        log_warn_ext("cannot free a NULL connection...\n");
        return CONN_NULL;
    }
#ifndef MOCK
    // close afu
    ocxl_afu_close(conn->afu);
#endif

    free(conn);
    return CONN_SUCCESS;
}

int add_conn(connection *conn) {
    if (clist == NULL) {
        clist = conn;
        return CONN_NULL;
    }

    if (get_conn(conn->circuit_id) != NULL) {
        log_warn("not allowed to replicate the connection - %s\n",
                 conn->circuit_id);
        return CONN_DUP;
    }

    connection *tmp = clist;
    clist = conn;
    conn->next = tmp;
    return CONN_SUCCESS;
}

int del_conn(const char *circuit_id) {
    if (circuit_id == NULL)
        return CONN_NULL;

    // case: list empty
    if (clist == NULL) {
        log_info("empty clist, returning...\n");
        return CONN_NULL;
    }

    connection *runner = clist;
    connection *prev = NULL;

    // we assume no replication of connections so
    // the circuit matching is the only one deleted
    while (runner != NULL) {
        if (strcmp(runner->circuit_id, circuit_id) == 0) {

            if (runner == clist) { // case the head is a match
                clist = runner->next;
            } else {
                prev->next = runner->next;
            }

#ifndef MOCK
            if (runner->stat_tid != NULL)
                if (stop_stats_thread(runner->stat_tid) != 0)
                    log_error_ext("Error stopping the stat thread\n");
#endif

            free_conn(runner);
            log_info_ext("conneciton %s freed successfully\n", circuit_id);
            return CONN_SUCCESS;
        }
        prev = runner;
        runner = runner->next;
    }
    return CONN_NOT_FOUND;
}

connection *get_conn(const char *circuit_id) {
    connection *runner = clist;
    while (runner != NULL) {
        if (strncmp(runner->circuit_id, circuit_id, CIRCUIT_ID_SIZE) == 0) {
            return runner;
        }
        runner = runner->next;
    }
    return runner;
}
int setup_afu_compute(connection *conn, uint64_t effective_addr,
                      iport_list *ports) {
#ifndef MOCK
    ocxl_err rc = ocxl_afu_open(conn->afu_name, &(conn->afu));
    if (OCXL_OK != rc) {
        log_error_ext("Could not open AFU '%s' error code %d\n", conn->afu_name,
                      rc);
        return ERR_AFU_OPEN;
    }

    log_info_ext("memapping global mmio space\n");
    rc = ocxl_mmio_map(conn->afu, OCXL_GLOBAL_MMIO, &(conn->global));
    if (rc != OCXL_OK) {
        log_error_ext("Could not map global");
        return ERR_MMIO_MAP_GLOBAL;
    }

    log_info_ext("Bringing up aurora channel- %x\n", ports->nport);
    rc = ocxl_mmio_write64(conn->global, AURORA_CTRL, OCXL_MMIO_LITTLE_ENDIAN,
                           ports->nport);
    if (rc != OCXL_OK) {
        log_error_ext("Failed to bring up the AURORA channel\n");
        return ERR_AURORA_CHAN;
    }

    log_info_ext("Brought up aurora channel\n");

    rc = ocxl_mmio_write64(
        conn->global, MEM_SEG0_CONFIG, OCXL_MMIO_LITTLE_ENDIAN,
        calculate_seg_offset(effective_addr, MEM_SEG0_OFFSET, ports->nport));

    if (rc != OCXL_OK) {
        log_error_ext("Failed to config memory segment");
        return ERR_ATTACH_SEG;
    }

    log_info_ext("Mapped afu port %d...\n", ports->nport);

    log_info_ext("Starting thymesis stats thread\n");
    start_stats_thread(&(conn->stat_tid), conn->global);

#endif
    return 0;
}

int setup_afu_memory(connection *conn) {
#ifndef MOCK
    // open connection to AFU
    if (OCXL_OK != ocxl_afu_open(conn->afu_name, &(conn->afu))) {
        log_error("Could not open AFU '%s'\n", conn->afu_name);
        return ERR_AFU_OPEN;
    }

    // Enable per-AFU verbose messages
    /*ocxl_afu_enable_messages((conn->afu), OCXL_ERRORS | OCXL_TRACING);*/
    log_info_ext("attaching afu to address space\n");
    if (OCXL_OK != ocxl_afu_attach((conn->afu), OCXL_ATTACH_FLAGS_NONE)) {
        log_error_ext("Could not attach afu");
        return ERR_AFU_ATTACH;
    }

    log_info_ext("mappin global mmio space\n");
    if (OCXL_OK !=
        ocxl_mmio_map((conn->afu), OCXL_GLOBAL_MMIO, &conn->global)) {
        log_error_ext("Could not map the AFU MMIO region");
        return ERR_MMIO_MAP_GLOBAL;
    }

    log_info_ext("mapping per process mmio space\n");
    ocxl_err rc =
        ocxl_mmio_map(conn->afu, OCXL_PER_PASID_MMIO, &(conn->pp_mmio));
    if (rc != OCXL_OK) {
        log_error_ext("Could not map per process local MMIO region");
        return ERR_MMIO_MAP_LOCAL;
    }

    global_mmio_control ctrl;
    ctrl.reg.test_enable = 1;
    ctrl.reg.trigger_sfw_intrp = 0;
    ctrl.reg.ignore_nomatch_on_read = 0;
    ctrl.reg.unassigned1 = 0;
    ctrl.reg.enable_pipeline = 0;

    log_info_ext("enabling and sending assign tag cmd\n");
    rc =
        ocxl_mmio_write64(conn->global, 0x0, OCXL_MMIO_LITTLE_ENDIAN, ctrl.val);
    if (rc != OCXL_OK) {
        log_error_ext("Failed to write ctrl register\n");
        return ERR_WRITE_CTRL_REGISTER;
    }
    ocxl_enable_messages(OCXL_ERRORS | OCXL_TRACING);

    log_info_ext("Enabling network\n");
    rc = ocxl_mmio_write64(conn->global, 0x78, OCXL_MMIO_LITTLE_ENDIAN, 0x1);
    if (rc != OCXL_OK) {
        log_error_ext("Failed enabling network\n");
        return ERR_ENABLING_NETWORK;
    }

#endif
    return 0;
}

void list_connections() {
    log_info_ext("--- BEGIN list circuits ---\n");
    connection *runner = clist;
    while (runner != NULL) {
        log_info_ext("circuit - id: %s\n", runner->circuit_id);
        runner = runner->next;
    }
    log_info_ext("--- END list circuits ---\n");
}
