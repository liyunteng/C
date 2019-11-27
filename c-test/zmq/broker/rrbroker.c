/*
 * rrbroker.c -- request-reply broker
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/27 12:55:42
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

int
main(void)
{
    void *context  = zmq_ctx_new();
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    void *backend  = zmq_socket(context, ZMQ_DEALER);
    zmq_bind(frontend, "tcp://*:5559");
    zmq_bind(backend, "tcp://*:5560");

    zmq_pollitem_t items[] = {
        {frontend, 0, ZMQ_POLLIN, 0},
        {backend, 0, ZMQ_POLLIN, 0},
    };

    while (1) {
        zmq_msg_t message;
        int more;

        zmq_poll(items, 2, -1);
        if (items[0].revents & ZMQ_POLLIN) {
            while (1) {
                zmq_msg_init(&message);
                zmq_msg_recv(&message, frontend, 0);
                printf("frontend recv: %s\n", (char *)zmq_msg_data(&message));
                size_t more_size = sizeof(more);
                zmq_getsockopt(frontend, ZMQ_RCVMORE, &more, &more_size);
                zmq_msg_send(&message, backend, more ? ZMQ_SNDMORE : 0);
                zmq_msg_close(&message);
                if (!more)
                    break;
            }
        }

        if (items[1].revents & ZMQ_POLLIN) {
            while (1) {
                zmq_msg_init(&message);
                zmq_msg_recv(&message, backend, 0);
                printf("backend: %s\n", (char *)zmq_msg_data(&message));
                size_t more_size = sizeof(more);
                zmq_getsockopt(backend, ZMQ_RCVMORE, &more, &more_size);
                zmq_msg_send(&message, frontend, more ? ZMQ_SNDMORE : 0);
                zmq_msg_close(&message);
                if (!more)
                    break;
            }
        }
    }
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
    return 0;
}
