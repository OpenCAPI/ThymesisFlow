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

#include "stats.h"
#include <libocxl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *_stats_thread(void *arg) {

    uint64_t tx, rx, latency;
    t_args *targs = (t_args *)arg;
    ocxl_err rc;

    int fd = open(THYMESISCOUNT_PATH, O_WRONLY);
    log_info_ext("successfully opened file: %s\n", THYMESISCOUNT_PATH);

    if (targs == NULL) {
        log_warn("%s: thread arguments are null\n", __func__);
        pthread_exit(NULL);
    }
    int cs;

    while (1) {
        if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) {
            log_error("error setting pthread_setcancelstate\n");
            exit(EXIT_FAILURE);
        }

        rc = ocxl_mmio_read64(targs->global, TX_STATS, OCXL_MMIO_LITTLE_ENDIAN,
                              &tx);
        if (rc != OCXL_OK) {
            log_info("Cannot read tx counter, exiting stat thread\n");
            close(fd);
            pthread_exit(NULL);
        }
        rc = ocxl_mmio_read64(targs->global, RX_STATS, OCXL_MMIO_LITTLE_ENDIAN,
                              &rx);
        if (rc != OCXL_OK) {
            log_info("Cannot read rx counter, exiting stat thread\n");
            close(fd);
            pthread_exit(NULL);
        }

        rc = ocxl_mmio_read64(targs->global, LATENCY_STATS,
                              OCXL_MMIO_LITTLE_ENDIAN, &latency);
        if (rc != OCXL_OK) {
            log_info("Cannot read latency counter, exiting stat thread\n");
            close(fd);
            pthread_exit(NULL);
        }

        // latency 64-bit register contains the average
        dprintf(fd, "%ld,%ld,%ld\n", tx, rx,
                (((1UL << 37) - 1) & latency) / (latency >> 37));

        cs = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        if (cs != 0) {
            log_error_ext("error setting pthread_setcancelstate\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
        sleep(targs->period);
    }
}

int tf_init_stats_thread(pthread_t *tid, ocxl_mmio_h global, int period) {

    t_args *targs;

    // Checking values
    if (tid == NULL) {
        fprintf(stderr, "%s: pthread_t is NULL\n", __func__);
        return -1;
    }
    if (global == NULL) {
        fprintf(stderr, "%s: global arg is NULL\n", __func__);
        return -1;
    }
    if (period <= 0) {
        fprintf(stderr, "%s: period is <= 0 \n", __func__);
        return -1;
    }

    targs = malloc(sizeof(t_args));
    if (targs == NULL) {
        fprintf(stderr, "%s: arg allocation failed\n", __func__);
    }

    targs->global = global;
    targs->period = period;

    return pthread_create(tid, NULL, _stats_thread, (void *)targs);
}

int start_stats_thread(pthread_t **tid, ocxl_mmio_h global) {

    unlink(THYMESISCOUNT_PATH);
    log_info_ext("opening thymesisstats file: %s\n", THYMESISCOUNT_PATH);
    int res = mkfifo(THYMESISCOUNT_PATH, 0666);
    if (res != 0) {
        log_error_ext("error opening fifo - err: %d\n", res);
        // set thread it to null so that we don't try to release it during while
        // we delete the connection
        *tid = NULL;
        return -1;
    }
    *tid = (pthread_t *)malloc(sizeof(pthread_t));

    return tf_init_stats_thread(*tid, global, PERIOD_SEC);
}

int stop_stats_thread(pthread_t *tid) {

    if (tid == NULL) {
        log_warn("stat thread reference cannot be null\n");
        return -1;
    }
    void *res;

    int s = pthread_cancel(*tid);
    if (s != 0) {
        log_error("error while cancelling stat thread\n");
        exit(EXIT_FAILURE);
    }

    /* Join with thread to see what its exit status was */
    s = pthread_join(*tid, &res);
    if (s != 0) {
        log_error("error while joining the stat thread");
        exit(EXIT_FAILURE);
    }
    if (res == PTHREAD_CANCELED) {
        log_info("stat thread successfully cancelled\n");
    } else
        log_error("stat thread wasn't cancelled, this should not happen\n");
    free(tid);
    return 0;
}
