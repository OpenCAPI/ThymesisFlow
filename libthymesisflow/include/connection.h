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

/// @file connection.h
#if !defined(CONNECTION_H)
#define CONNECTION_H

#include "errcode.h"
#include "iport.h"
#include "logger.h"
#include "protoconst.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MOCK
#include "libocxl.h"

#define AURORA_CTRL 0x78

#define AURORA_CHAN 0x02

#define MEM_SEG0_CONFIG 0x20

#define MEM_SEG0_OFFSET 0x0

#define MEM_SEG1_CONFIG 0x28

#define MEM_SEG1_OFFSET (1UL << 40)
#include "stats.h"

typedef union {
    struct {
        unsigned int test_enable : 1;
        unsigned int trigger_sfw_intrp : 1;
        unsigned int ignore_nomatch_on_read : 1;
        unsigned int unassigned1 : 1;
        unsigned int enable_pipeline : 1;
    } reg;
    uint64_t val;
} global_mmio_control;

#endif

/**
 * Contains all variables related to an active remote memory allocation
 */
struct connection_s {
    char circuit_id[CIRCUIT_ID_SIZE + 1];
    char afu_name[AFUNAME_SIZE];
    uint64_t size;
    unsigned char *ea;
    struct connection_s *next;

#ifndef MOCK
    ocxl_afu_h afu;
    pthread_t *stat_tid;
    ocxl_mmio_h global;
    ocxl_mmio_h pp_mmio;
#endif
};

typedef struct connection_s connection;

connection *clist;

/**
 * Add a new connection
 *
 * @param[in] connection: connection to be added
 * @param[out] outcome: 0 if operations finished with success
 */
int add_conn(connection *conn);

/**
 * Get connection from circuit_id
 *
 * @param[in] circuit_id: circuit identifier
 * @param[out] connection: first connection having the specified circuit id
 */
connection *get_conn(const char *circuit_id);

/**
 * Delete first circuit matching cirguit_id
 *
 * @param[in] circuit_id: circuit identifier
 * @param[out] outcome: 0 if operations finished with success
 */
int del_conn(const char *circuit_id);

/**
 * List all active connections
 */
void list_connections();

/**
 * Free dynamically allocated connection struc
 *
 * @param[in] connection: connection to be freed
 * @param[out] outcome: 0 if operations finished with success
 */
int free_conn(connection *conn);

/**
 * Dynamically allocate a new connection
 *
 * @param[in] circuit_id: circuit identifier
 * @param[in] afu_name: AFU to be used
 * @param[in] size: disaggregated memory size
 * @param[out] connection: connection to be freed
 */
connection *new_conn(const char *circuit_id, const char *afu_name,
                     const uint64_t size);

int setup_afu_compute(connection *conn, uint64_t effective_addr,
                      iport_list *ports);

int setup_afu_memory(connection *conn);

#define CONN_SUCCESS 0

#define CONN_DUP 1
#define CONN_NULL 2

#define CONN_NOT_FOUND 3

#endif
