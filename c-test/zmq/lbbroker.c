/*
 * lbbroker.c -- Load-balancing broker
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/31 18:47:46
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

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <zmq.h>

#define NBR_CLIENS 10
#define NBR_WORKERS 3

#define DEQUEUE(q) memmove(&(q)[0], &(q)[1], sizeof(q) - sizeof(q[0]))

static void *
client_task(void *args)
{
    void *context = zmq_ctx_new();

    void *client = zmq_socket(context, ZMQ_REQ);
    char  id[256];
    snprintf(id, sizeof(id), "%lu", pthread_self());
    zmq_setsockopt(client, ZMQ_IDENTITY, id, strlen(id) + 1);

    zmq_connect(client, "ipc://frontend.ipc");

    zmq_send(client, "HELLO", 6, 0);
    char buf[256];
    zmq_recv(client, buf, sizeof(buf), 0);
    printf("%lu Client: %s\n", pthread_self(), buf);
    zmq_close(client);
    zmq_ctx_destroy(context);
    return NULL;
}

static void *
worker_task(void *args)
{
    void *context = zmq_ctx_new();

    void *worker = zmq_socket(context, ZMQ_REQ);
    char  id[256];
    snprintf(id, sizeof(id), "%lu", pthread_self());
    zmq_setsockopt(worker, ZMQ_IDENTITY, id, strlen(id) + 1);

    zmq_connect(worker, "ipc://backend.ipc");

    zmq_send(worker, "READY", 6, 0);
    while (1) {
        char p_id[256];
        char buf[256];
        zmq_recv(worker, p_id, sizeof(p_id), 0);
        zmq_recv(worker, buf, sizeof(buf), 0);
        zmq_recv(worker, buf, sizeof(buf), 0);
        printf("%lu Worker: %s\n", pthread_self(), buf);

        zmq_send(worker, p_id, strlen(p_id) + 1, ZMQ_SNDMORE);
        zmq_send(worker, "", 0, ZMQ_SNDMORE);
        zmq_send(worker, "OK", 3, 0);
    }

    zmq_close(worker);
    zmq_ctx_destroy(context);
    return NULL;
}

int
main(void)
{
    void *context  = zmq_ctx_new();
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    void *backend  = zmq_socket(context, ZMQ_ROUTER);

    zmq_bind(frontend, "ipc://frontend.ipc");
    zmq_bind(backend, "ipc://backend.ipc");

    int client_nbr;
    for (client_nbr = 0; client_nbr < NBR_CLIENS; client_nbr++) {
        pthread_t client;
        pthread_create(&client, NULL, client_task, NULL);
    }

    int worker_nbr;
    for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr++) {
        pthread_t worker;
        pthread_create(&worker, NULL, worker_task, NULL);
    }

    int  available_worker = 0;
    char worker_queue[10][256];

    while (1) {
        zmq_pollitem_t items[] = {
            {backend, 0, ZMQ_POLLIN, 0},
            {frontend, 0, ZMQ_POLLIN, 0},
        };

        int rc = zmq_poll(items, available_worker ? 2 : 1, -1);
        if (rc == -1)
            break;

        if (items[0].revents & ZMQ_POLLIN) {
            char id[256], buf[256];
            zmq_recv(backend, worker_queue[available_worker], sizeof(worker_queue[0]), 0);
            available_worker++;

            zmq_recv(backend, buf, sizeof(buf), 0);
            zmq_recv(backend, id, sizeof(id), 0);
            if (strcmp(id, "READY") != 0) {
                zmq_recv(backend, buf, sizeof(buf), 0);
                zmq_recv(backend, buf, sizeof(buf), 0);

                zmq_send(frontend, id, strlen(id) + 1, ZMQ_SNDMORE);
                zmq_send(frontend, "", 0, ZMQ_SNDMORE);
                zmq_send(frontend, buf, strlen(buf) + 1, 0);

                if (--client_nbr == 0)
                    break;
            }
        }

        if (items[1].revents & ZMQ_POLLIN) {
            char id[256], buf[256];
            zmq_recv(frontend, id, sizeof(id), 0);
            zmq_recv(frontend, buf, sizeof(buf), 0);
            zmq_recv(frontend, buf, sizeof(buf), 0);

            zmq_send(backend, worker_queue[0], strlen(worker_queue[0]) + 1, ZMQ_SNDMORE);
            zmq_send(backend, "", 0, ZMQ_SNDMORE);
            zmq_send(backend, id, strlen(id) + 1, ZMQ_SNDMORE);
            zmq_send(backend, "", 0, ZMQ_SNDMORE);
            zmq_send(backend, buf, strlen(buf) + 1, 0);

            DEQUEUE(worker_queue);
            available_worker--;
        }
    }

    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
    return 0;
}
