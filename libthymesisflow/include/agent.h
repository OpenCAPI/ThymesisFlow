/// @file agent.h
#if !defined(THYMESISFLOW_CMD_H)
#define THYMESISFLOW_CMD_H

#include <sys/socket.h>
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
#include "proto.h"

#ifndef MOCK
#include "stats.h"
#endif

/*! \def BACKLOG
 * \brief max incoming connections
 */
#define BACKLOG 5


void run_agent(const char* sock_path);

#endif
