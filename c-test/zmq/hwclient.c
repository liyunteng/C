/*
 * hwclient.c -- hello world
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/25 17:54:04
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
#include <unistd.h>
#include <zmq.h>

int
main(int argc, char *argv[])
{
    void *context = zmq_ctx_new();

    /* Socket to talk to server */
    printf("Connecting to hello world server...\n");
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        /*
         * zmq_msg_t request;
         * zmq_msg_init_size(&request, 5);
         * memcpy(zmq_msg_data(&request), "Hello", 5);
         * printf("Sending Hello %d...\n", request_nbr);
         * zmq_msg_send(&request, requester, 0);
         * zmq_msg_close(&request);
         */

        int ret = zmq_send(requester, "hello", 5, 0);
        assert(ret == 5);

        char      buf[10];
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, requester, 0);
        memset(buf, 0, sizeof(buf));
        memcpy(buf, zmq_msg_data(&reply), zmq_msg_size(&reply));
        printf("Received %s %d\n", buf, request_nbr);
        zmq_msg_close(&reply);
    }

    sleep(2);
    zmq_close(requester);
    zmq_ctx_destroy(context);

    return 0;
}
