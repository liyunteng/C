/*
 * tasksink.c -- task sink
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/26 10:24:44
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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <zmq.h>

int
main(void)
{
    void *context  = zmq_ctx_new();
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_bind(receiver, "tcp://*:5558");

    char buf[16];
    zmq_recv(receiver, buf, sizeof(buf), 0);

    struct timeval tv;
    int ret;
    ret = gettimeofday(&tv, NULL);
    assert(ret == 0);

    int64_t start_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    int task_nbr;
    for (task_nbr = 0; task_nbr != 100; task_nbr++) {
        zmq_recv(receiver, buf, sizeof(buf), 0);
        if (task_nbr % 10 == 0) {
            printf(":");
        } else
            printf(".");
        fflush(stdout);
    }

    ret = gettimeofday(&tv, NULL);
    assert(ret == 0);
    printf("\nTotal elapsed time: %d msec\n",
           (int)(tv.tv_sec * 1000 + tv.tv_usec / 1000 - start_time));

    zmq_close(receiver);
    zmq_ctx_destroy(context);
    return 0;
}
