/*
 * zhelpers.h -- helper header file for example
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/05/28 12:36:59
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

#ifndef ZHELPERS_H
#define ZHELPERS_H

#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <zmq.h>

#define randof(num) (int)((float)(num)*random() / (RAND_MAX + 1.0))

static char *
s_recv(void *socket)
{
    char buffer[256];
    int size = zmq_recv(socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    if (size > 255)
        size = 255;
    buffer[size] = 0;
    return strdup(buffer);
}

static int
s_send(void *socket, char *string)
{
    int size = zmq_send(socket, string, strlen(string), 0);
    return size;
}

static void
s_dump(void *socket)
{
    puts("----------------------------");
    while (1) {
        zmq_msg_t message;
        zmq_msg_init(&message);
        int size = zmq_msg_recv(&message, socket, 0);

        char *data  = (char *)zmq_msg_data(&message);
        int is_text = 1;
        int char_nbr;
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            if ((unsigned char)data[char_nbr] < 32
                || (unsigned char)data[char_nbr] > 127)
                is_text = 0;
        }

        printf("[%03d] ", size);
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            if (is_text)
                printf("%c", data[char_nbr]);
            else
                printf("%02X", (unsigned char)data[char_nbr]);
        }
        printf("\n");

        int more         = 0;
        size_t more_size = sizeof(more);
        zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
        zmq_msg_close(&message);
        if (!more)
            break;
    }
}

static void
s_set_id(void *socket)
{
    char identity[10];
    sprintf(identity, "%04X-%04X", randof(0x10000), randof(0x10000));
    zmq_setsockopt(socket, ZMQ_IDENTITY, identity, strlen(identity));
}

static void
s_sleep(int msecs)
{
    struct timespec t;
    t.tv_sec  = msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep(&t, NULL);
}

static int64_t
s_clock(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static void
s_console(const char *format, ...)
{
    time_t curtime     = time(NULL);
    struct tm *loctime = localtime(&curtime);
    char *formatted    = (char *)malloc(20);
    strftime(formatted, 20, "%y-%m-%d %H:%M:%S", loctime);
    printf("%s", formatted);
    free(formatted);

    va_list argptr;
    va_start(argptr, format);
    vprintf(format, argptr);
    va_end(argptr);
    printf("\n");
}
#endif
