#ifndef THYMESISSTATS_H
#define THYMESISSTATS_H

#include "logger.h"
#include <fcntl.h>
#include <libocxl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#define TX_STATS 0x60
#define RX_STATS 0x68
#define LATENCY_STATS 0x70
#define PERIOD_SEC 1
#define THYMESISCOUNT_PATH "/tmp/thymesispcount"

struct thread_args {
    ocxl_mmio_h global;
    int fd;
    int period;
};

typedef struct thread_args t_args;

// int  tf_init_stats_thread(pthread_t *  tid, int fd, ocxl_mmio_h global, int
// period);
int start_stats_thread(pthread_t **tid, ocxl_mmio_h global);
int stop_stats_thread(pthread_t *tid);

#endif
