/// @file server.h
#if !defined(THYMESISFLOW_CMD_H)
#define THYMESISFLOW_CMD_H

#include <sys/socket.h>

#ifndef MOCK
#include "stats.h"
#endif

/*! \def BACKLOG
 * \brief max incoing connections
 */
#define BACKLOG 5

#endif
