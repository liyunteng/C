/*
 * rrclient.c -- hello world client
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/27 12:59:42
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

int main(void)
{
        void *context = zmq_ctx_new();
        void *requester = zmq_socket(context, ZMQ_REQ);
        zmq_setsockopt(requester, ZMQ_IDENTITY, "lyt", 4);
        zmq_connect(requester, "tcp://localhost:5559");

        int request_nbr;
        char buf[10];
        for (request_nbr = 0; request_nbr != 10; request_nbr ++) {
                zmq_send(requester, "Hello", 6, 0);
                zmq_recv(requester, buf, sizeof(buf), 0);
                printf("recv: %d [%s]\n", request_nbr, buf);
        }

        zmq_close(requester);
        zmq_ctx_destroy(context);
        return 0;
}
