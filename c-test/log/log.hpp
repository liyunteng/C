/*
 * log.hpp -- log
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/12/13 08:49:44
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

#ifndef LOG_HPP
#define LOG_HPP
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

// #define DEBUG
// #define LOG_SOCKET

#define LOG_DEBUG         0
#define LOG_INFO          1
#define LOG_WARN          2
#define LOG_ERROR         3
#define LOG_FATAL         4

#define LOGFILE "uac.log"
#define LOGFILEBAK "uac.log.bak"

#define LOG_DEBUGFPRINTF(__fp__, __format__, __timebuf__, __level__, ...) fprintf(__fp__, "%s %18.18s(%4d) [%5s]" __format__ "\n", \
                                                                                  __timebuf__, __FILE__, __LINE__, LOGLEVEL[__level__], \
                                                                                  ##__VA_ARGS__)

#define LOG_FPRINTF(__fp__, __format__, ...) fprintf(__fp__, "" __format__ "\n",  ##__VA_ARGS__)

static int __LOGLEVEL__ = 0;
static inline void get_time(char *buf, size_t len)
{
        memset(buf, 0, len);
        time_t t= time(NULL);
        tm *now = localtime(&t);
        sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", now->tm_year+1990,
                now->tm_mon+1, now->tm_mday, now->tm_hour,
                now->tm_min, now->tm_sec);

}

#ifndef LOG_SOCKET
static pthread_mutex_t __LOGMUTEX__ = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef LOG_SOCKET
#define DEBUG
#endif

#ifdef DEBUG
static const char* LOGLEVEL[] = {
        "DEBUG",
        "INFO",
        "WARNING"
        "ERROR",
        "FATAL",
};

#ifdef LOG_SOCKET
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#define LOGSERVER "127.0.0.1"
#define LOGPORT 34567
#define LOG(__level__, __format__, ...)                                 \
        do {                                                            \
                if (__level__ >= __LOGLEVEL__) {                        \
                        int __logsockfd = -1;                           \
                        struct sockaddr_in __logaddr;                   \
                        char __logbuf[24];                              \
                        char __logsendbuf[1024];                        \
                                                                        \
                        __logsockfd = socket(AF_INET, SOCK_DGRAM, 0);   \
                        if (__logsockfd == -1) {                        \
                                get_time(__logbuf, sizeof(__logbuf));   \
                                fprintf(stderr, "%s create socket failed: %s\n", __logbuf, strerror(errno)); \
                                break;                                  \
                        }                                               \
                                                                        \
                        memset(&__logaddr, 0, sizeof(__logaddr));       \
                        __logaddr.sin_family = AF_INET;                 \
                        __logaddr.sin_port = htons(LOGPORT);            \
                        if (inet_pton(AF_INET, LOGSERVER, &__logaddr.sin_addr) < 0) { \
                                get_time(__logbuf, sizeof(__logbuf));   \
                                fprintf(stderr, "%s inet_pton failed: %s\n", __logbuf, strerror(errno)); \
                                close(__logsockfd);                     \
                                __logsockfd = -1;                       \
                        }                                               \
                                                                        \
                        get_time(__logbuf, sizeof(__logbuf));           \
                        snprintf(__logsendbuf, sizeof(__logsendbuf), "%s %11s(%4d) [%5s] " __format__ "\n", \
                                 __logbuf, __FILE__, __LINE__, LOGLEVEL[__level__], ##__VA_ARGS__); \
                        socklen_t addrlen = sizeof(__logaddr);          \
                        sendto(__logsockfd, __logsendbuf, sizeof(__logsendbuf), \
                               0, (struct sockaddr *)&__logaddr, addrlen); \
                        printf("%s", __logsendbuf);                     \
                        close(__logsockfd);                             \
                }                                                       \
        } while(0)

// logclient
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/types.h>
// #include <errno.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdio.h>

// #define LOGPORT 34567

// int main(int argc, char *argv[])
// {
//         int sockfd;
//         struct sockaddr_in addr;
//         struct sockaddr_in srcaddr;
//         char buf[1024];

//         sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//         if (sockfd < 0) {
//                 perror("socket");
//                 return sockfd;
//         }

//         memset(&addr, 0, sizeof(addr));
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(LOGPORT);
//         addr.sin_addr.s_addr = htonl(INADDR_ANY);

//         if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
//                 perror("bind");
//                 return -1;
//         }

//         socklen_t addrlen;
//         while(recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&srcaddr, &addrlen)) {
//                 fprintf(stderr, "from: %s recv: %s",inet_ntoa(srcaddr.sin_addr), buf);
//         }

//         return 0;
// }
#else  // end of (DEBUG and LOG_SOCKET)

#define LOG(__level__, __format__, ...)                                 \
        do {                                                            \
                if (__level__ >= __LOGLEVEL__) {                        \
                        char __timebuf[24];                             \
                        get_time(__timebuf, sizeof(__timebuf));         \
                        pthread_mutex_lock(&__LOGMUTEX__);              \
                        FILE *__fp = fopen(LOGFILE, "a+");              \
                        if (__fp != NULL) {                             \
                                LOG_DEBUGFPRINTF(__fp, __format__, __timebuf, __level__, ##__VA_ARGS__); \
                                fclose(__fp);                           \
                        }                                               \
                        pthread_mutex_unlock(&__LOGMUTEX__);            \
                        LOG_DEBUGFPRINTF(stderr, __format__, __timebuf, __level__, ##__VA_ARGS__); \
                }                                                       \
        } while(0)
#endif  // end of (DEBUG and !LOG_SOCKET)

#else

#define LOG(__level__, __format__, ...)                                 \
        do {                                                            \
                if (__level__ >= __LOGLEVEL__) {                        \
                        pthread_mutex_lock(&__LOGMUTEX__);              \
                        FILE *__fp = fopen(LOGFILE, "a+");              \
                        if (__fp != NULL ) {                            \
                                LOG_FPRINTF(__fp, __format__, ##__VA_ARGS__); \
                                fclose(__fp);                           \
                        }                                               \
                        pthread_mutex_unlock(&__LOGMUTEX__);            \
                        LOG_FPRINTF(stderr, __format__, ##__VA_ARGS__); \
                }                                                       \
        } while(0)

#endif  // not DEBUG

#define LOG_INIT()                                      \
        do {                                            \
                if(!access(LOGFILE, F_OK)) {            \
                        rename(LOGFILE, LOGFILEBAK);    \
                }                                       \
                FILE *__fp = fopen(LOGFILE, "w");       \
                if (__fp != NULL) {                     \
                        fclose(__fp);                   \
                        __fp = NULL;                    \
                }                                       \
        } while (0)

#endif
