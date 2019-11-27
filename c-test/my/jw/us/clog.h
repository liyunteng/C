#ifndef __CLOG_H
#define __CLOG_H
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#ifdef DEBUG
#    define CLOG_OPT (LOG_PERROR)
#else
#    define CLOG_OPT (LOG_CONS)
#endif

#define clog_init() openlog("US", CLOG_OPT, LOG_DAEMMON)
#define clog_release() closelog()
#define clog syslog

#define die(fmt, ...)                                                          \
    do {                                                                       \
        clog(LOG_EMERG, "%s %s:" fmt, __FILE__, __func__, ##__VA_ARGS__);      \
        exit(1);                                                               \
    } while (0)

enum {
    CL_ERROR = 0,
    CL_WARN,
    CL_INFO,
    CL_DEBUG,
    CL_DEBUG_V,
    CL_MAX  = CL_DEBUG_V,
    CL_NONE = -1,
};

#define cdbg(level, fmt, ...)                                                  \
    do {                                                                       \
        if (level <= clog_level)                                               \
            fprintf(stderr, fmt, ##__VA_ARGS__);                               \
    } while (0)

#endif  // __CLOG_H
