/*
 * log.h -- log
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/12/16 21:50:01
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>

#define VERBOSE_TIMEFORMAT "%Y-%m-%d %H:%M:%S"

#define MAX_BAK 4
#define MAX_FILESIZE 10*1024*1024
#define MAX_OPENFILE 256
#define BUFSZ 4096

// #define FILEPERM  (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

#define VERBOSE
// #define SYSLOG
// #define SOCKLOG
// #define STDERRLOG

#ifdef SOCKLOG
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#define LOGSERVER "127.0.0.1"
#define LOGPORT 34567
#endif

#ifdef SYSLOG
#include <syslog.h>
#endif

typedef enum {
        LOGLEVEL_NONE,
        LOGLEVEL_EMERG,
        LOGLEVEL_ALERT,
        LOGLEVEL_CRIT,
        LOGLEVEL_ERROR,
        LOGLEVEL_WARNING,
        LOGLEVEL_NOTICE,
        LOGLEVEL_INFO,
        LOGLEVEL_DEBUG,
#ifdef SYSLOG
#define LOG_NONE        LOGLEVEL_NONE
#define LOG_FATAL       LOG_CRIT
#define LOG_ERROR       LOG_ERR
#else
#define LOG_NONE        LOGLEVEL_NONE
#define LOG_EMERG       LOGLEVEL_EMERG
#define LOG_ALERT       LOGLEVEL_ALERT
#define LOG_FATAL       LOGLEVEL_CRIT
#define LOG_CRIT        LOGLEVEL_CRIT
#define LOG_ERROR       LOGLEVEL_ERROR
#define LOG_WARNING     LOGLEVEL_WARNING
#define LOG_NOTICE      LOGLEVEL_NOTICE
#define LOG_INFO        LOGLEVEL_INFO
#define LOG_DEBUG       LOGLEVEL_DEBUG
#endif
} LOGLEVEL;


typedef struct {
        FILE *fp;
        // int fd;
        char logfile[256];
        pthread_mutex_t mutex;

        uint64_t success_count;
        uint64_t success_byte;
        size_t fprintf_fail_count;
        size_t makebak_count;
        uint64_t debug_count;
        uint64_t info_count;
        uint64_t warning_count;
        uint64_t error_count;
        uint64_t fatal_count;
        uint64_t alert_count;
        uint64_t emerg_count;
        uint64_t unhandle_count;
}logfp;

typedef struct _loger loger;
struct _loger {
        LOGLEVEL loglevel;
        logfp *lfp;
#ifdef SOCKLOG
        int sockfd;
        struct sockaddr_in addr;
#endif
};

extern pthread_mutex_t logmutex;
extern loger sloger;
loger *log_create(const char *file, LOGLEVEL level);
void mlog(loger *handle, LOGLEVEL level, const char *file, size_t filelen, const char *function, size_t functionlen, long line, const char *format, ...);
void vlog(loger *handle, LOGLEVEL level, const char *file, size_t filelen, const char *function, size_t functionlen, long line, const char *format, va_list args);
void log_dump();

#define DEBUG(handle, format, ...)                                      \
        mlog(handle, LOGLEVEL_DEBUG,   __FILE__, sizeof(__FILE__), __FUNCTION__, sizeof(__FUNCTION__), __LINE__, format, ##__VA_ARGS__);
#define INFO(handle, format, ...)                                       \
        mlog(handle, LOGLEVEL_INFO,    __FILE__, sizeof(__FILE__), __FUNCTION__, sizeof(__FUNCTION__), __LINE__, format, ##__VA_ARGS__);
#define WARNING(handle, format, ...)                                    \
        mlog(handle, LOGLEVEL_WARNING, __FILE__, sizeof(__FILE__), __FUNCTION__, sizeof(__FUNCTION__), __LINE__, format, ##__VA_ARGS__);
#define ERROR(handle, format, ...)                                      \
        mlog(handle, LOGLEVEL_ERROR,   __FILE__, sizeof(__FILE__), __FUNCTION__, sizeof(__FUNCTION__), __LINE__, format, ##__VA_ARGS__);
#define FATAL(handle, format, ...)                                      \
        mlog(handle, LOGLEVEL_CRIT,    __FILE__, sizeof(__FILE__), __FUNCTION__, sizeof(__FUNCTION__), __LINE__, format, ##__VA_ARGS__);
#define ALERT(handle, format, ...)                                      \
        mlog(handle, LOGLEVEL_ALERT,   __FILE__, sizeof(__FILE__), __FUNCTION__, sizeof(__FUNCTION__), __LINE__, format, ##__VA_ARGS__);
#define EMERG(handle, format, ...)                                      \
        mlog(handle, LOGLEVEL_EMERG,   __FILE__, sizeof(__FILE__), __FUNCTION__, sizeof(__FUNCTION__), __LINE__, format, ##__VA_ARGS__);



int slog_init(const char *file, LOGLEVEL level);
void slog(LOGLEVEL level, const char * file, size_t filelen, const char * function, size_t functionlen, long line, const char *format, ...);
#define LOG(level, format, ...)                                         \
        slog(level, __FILE__, sizeof(__FILE__), __FUNCTION__, sizeof(__FUNCTION__), __LINE__, format, __VA_ARGS__);
#define LOG_INIT(file, level)                   \
        slog_init(file, level);
#endif
