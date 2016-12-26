/*
 * 4taskwork2.c -- task worker - design 2
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/28 12:21:00
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
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
        void *context = zmq_ctx_new();

        void *receiver = zmq_socket(context, ZMQ_PULL);
        zmq_connect(receiver, "tcp://localhost:5557");

        void *sender = zmq_socket(context, ZMQ_PUSH);
        zmq_connect(sender, "tcp://localhost:5558");

        void *controller = zmq_socket(context, ZMQ_SUB);
        zmq_connect(controller, "tcp://localhost:5559");
        zmq_setsockopt(controller,ZMQ_SUBSCRIBE, "", 0);


        zmq_pollitem_t items[] = {
                {receiver, 0, ZMQ_POLLIN, 0},
                {controller, 0, ZMQ_POLLIN, 0},
        };

        while (1) {
                zmq_msg_t message;
                zmq_poll(items, 2, -1);
                if (items[0].revents &  ZMQ_POLLIN) {
                        zmq_msg_init(&message);
                        zmq_msg_recv(&message, receiver,0);

                        usleep(atoi((char *)zmq_msg_data(&message)));

                        zmq_msg_init(&message);
                        zmq_msg_send(&message, sender, 0);

                        printf(".");
                        fflush(stdout);

                        zmq_msg_close(&message);
                }
                if (items[1].revents & ZMQ_POLLIN) {
                        break;
                }
        }

        zmq_close(receiver);
        zmq_close(sender);
        zmq_close(controller);
        zmq_ctx_destroy(context);
        return 0;
}
