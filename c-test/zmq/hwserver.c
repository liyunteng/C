/*
 * hwserver.c -- hello world
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/25 17:50:28
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
main(int argc, char *argv[])
{
    void *context = zmq_ctx_new();

    /* Socket to talk to client */
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://*:5555");

    while (1) {
        /* wait for next request from client */
        char buf[10];
        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_msg_recv(&request, responder, 0);
        memset(buf, 0, sizeof(buf));
        memcpy(buf, zmq_msg_data(&request), zmq_msg_size(&request));
        printf("Received %s\n", buf);
        zmq_msg_close(&request);

        sleep(1);

        /* send reply back to client */
        zmq_msg_t reply;
        zmq_msg_init_size(&reply, 5);
        memcpy(zmq_msg_data(&reply), "world", 5);
        zmq_msg_send(&reply, responder, 0);
        zmq_msg_close(&reply);
    }

    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}
