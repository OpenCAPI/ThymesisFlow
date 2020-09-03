/// @file logger.h

#ifndef LOGGER_H
#define LOGGER_H

#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define ERROR_TAG "ERROR"
#define WARN_TAG "WARN"
#define INFO_TAG "INFO"
#define DEBUG_TAG "DEBUG"

/**
 * Get current time
 *
 * @param[out] time_string: string reporting the time as %Y-%m-%d %H:%M:%S
 */
static inline char *current_time() {
    static char time_string[64];
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_string, 64, "%Y-%m-%d %H:%M:%S", timeinfo);

    return time_string;
}

static inline void print_log(const char *tag, const char *curr_time,
                             const char *message, va_list args) {
    printf("%s [%s] ", curr_time, tag);
    vprintf(message, args);
    fflush(stdout); // printf("\n");
}

static inline void log_info(char *message, ...) {
    va_list args;
    va_start(args, message);
    print_log(INFO_TAG, current_time(), message, args);
    va_end(args);
}
static inline void log_debug(char *message, ...) {
    va_list args;
    va_start(args, message);
    print_log(DEBUG_TAG, current_time(), message, args);
    va_end(args);
}

static inline void log_warn(char *message, ...) {
    va_list args;
    va_start(args, message);
    print_log(WARN_TAG, current_time(), message, args);
    va_end(args);
}

static inline void log_error(char *message, ...) {
    va_list args;
    va_start(args, message);
    print_log(ERROR_TAG, current_time(), message, args);
    va_end(args);
}

static inline void print_log_ext(const char *tag, const char *curr_time,
                                 const char *filename, int line,
                                 const char *message, ...) {
    va_list args;
    va_start(args, message);
    printf("%s [%s] %s:%d, ", curr_time, tag, filename, line);
    vprintf(message, args);
    va_end(args);
    fflush(stdout);
}

#define log_info_ext(message, args...)                                         \
    print_log_ext(INFO_TAG, current_time(), basename(__FILE__), __LINE__,      \
                  message, ##args)

#define log_debug_ext(message, args...)                                        \
    print_log_ext(DEBUG_TAG, current_time(), basename(__FILE__), __LINE__,     \
                  message, ##args)

#define log_warn_ext(message, args...)                                         \
    print_log_ext(WARN_TAG, current_time(), basename(__FILE__), __LINE__,      \
                  message, ##args)

#define log_error_ext(message, args...)                                        \
    print_log_ext(ERROR_TAG, current_time(), basename(__FILE__), __LINE__,     \
                  message, ##args)

#endif
