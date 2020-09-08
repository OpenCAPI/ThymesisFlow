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

/// @file iport.c
#include "iport.h"

void cycle_ports(iport_list *pl) {
    iport_list *plrunner = pl;
    while (plrunner) {
        log_info("selected port: %d\n", plrunner->nport);
        plrunner = plrunner->next;
    }
}

void free_iport_list(iport_list *pl) {

    while (pl) {
        iport_list *pl_next = pl->next;
        free(pl);
        pl = pl_next;
    }
}

iport_list *add_port(iport_list *pl, u_int8_t port_number) {
    if (pl == NULL) {
        pl = (iport_list *)malloc(sizeof(iport_list));
        pl->nport = port_number;
        pl->next = NULL;
        return pl;
    }
    iport_list *new_plhead = (iport_list *)malloc(sizeof(iport_list));
    new_plhead->nport = port_number;
    new_plhead->next = pl;
    return new_plhead;
}

u_int8_t marshal_iports(const iport_list *pl) {

    u_int8_t port_rep = 0;

    const iport_list *plrunner = pl;

    while (plrunner) {
        port_rep += (1 << (plrunner->nport - 1));
        plrunner = plrunner->next;
    }
    return port_rep;
}

iport_list *unmarshal_iports(u_int8_t n) {

    u_int8_t i = 1, pos = 1;

    iport_list *pl = NULL;

    while (i) {

        if (i & n) {
            pl = add_port(pl, pos);
        }

        // Unset current bit and set the next bit in 'i'
        i = i << 1;

        // increment position
        ++pos;
    }

    return pl;
}
