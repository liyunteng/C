/*
 * rtreq.c -- ROUTER-to-REQ example
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/31 13:44:26
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
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>


#define NBR_WORKERS 10

static void *
worker_task(void *args)
{
        void *context = zmq_ctx_new();
        void *worker = zmq_socket(context, ZMQ_REQ);

        char id[256];
        snprintf(id, sizeof(id),  "%lu", pthread_self());
        zmq_setsockopt(worker, ZMQ_IDENTITY, &id, strlen(id)+1);

        zmq_connect(worker, "tcp://localhost:5671");

        int total = 0;
        while (1) {
                zmq_send(worker, "Hi", 3, 0);

                char buf[256];
                zmq_recv(worker, buf, sizeof(buf), 0);
                int finished = (strcmp(buf, "Fired!") == 0);
                if (finished) {
                        printf("%lu Completed: %d tasks\n", pthread_self(), total);
                        break;
                }

                total ++;
                usleep ((random() % 500 + 1) * 1000);
        }

        zmq_close(worker);
        zmq_ctx_destroy(context);
        return NULL;
}


int main(void)
{
        void *context = zmq_ctx_new();
        void *broker = zmq_socket(context, ZMQ_ROUTER);

        zmq_bind(broker, "tcp://*:5671");
        srandom((unsigned)time(NULL));

        int worker_nbr;
        for(worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr ++) {
                pthread_t worker;
                pthread_create(&worker, NULL, worker_task, NULL);
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t start_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        int64_t end_time = start_time + 5000;


        int workers_fired = 0;
        while (1) {
                char buf[256];
                zmq_recv(broker, buf, sizeof(buf), 0);
                zmq_send(broker, buf, strlen(buf) + 1, ZMQ_SNDMORE);
                zmq_recv(broker, buf, sizeof(buf), 0);
                zmq_recv(broker, buf, sizeof(buf), 0);
                zmq_send(broker, "", 0, ZMQ_SNDMORE);

                gettimeofday(&tv, NULL);
                if (tv.tv_sec * 1000 + tv.tv_usec / 1000 < end_time) {
                        zmq_send(broker, "good", 5, 0);
                } else {
                        zmq_send(broker, "Fired!", 7, 0);
                        if (++workers_fired == NBR_WORKERS)
                                break;
                }
        }

        zmq_close(broker);
        zmq_ctx_destroy(context);
        return 0;
}
