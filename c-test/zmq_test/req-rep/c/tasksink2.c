/*
 * tasksink2.c -- task sink - design 2
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/28 13:01:05
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

#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/time.h>

int main(void)
{
        void *context = zmq_ctx_new();

        void *receiver = zmq_socket(context, ZMQ_PULL);
        zmq_bind(receiver, "tcp://*:5558");

        void *controller = zmq_socket(context, ZMQ_PUB);
        zmq_bind(controller, "tcp://*:5559");

        char buf[256];
        zmq_recv(receiver, buf, sizeof(buf), 0);

        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t start_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

        int task_nbr;
        for (task_nbr = 0; task_nbr < 100; task_nbr ++) {
                zmq_recv(receiver, buf, sizeof(buf), 0);
                if (task_nbr % 10 == 0) {
                        printf(":");
                } else {
                        printf(".");
                }
                fflush(stdout);
        }

        gettimeofday(&tv, NULL);
        printf("Total elapsed time: %d msec\n",
               (int) ((tv.tv_sec * 1000 + tv.tv_usec / 1000) - start_time));

        zmq_send(controller, "KILL", 5, 0);

        sleep(1);

        zmq_close(receiver);
        zmq_close(controller);
        zmq_ctx_destroy(context);
        return 0;
}
