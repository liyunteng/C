/*
 * rrserver.c -- hello world server
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/27 12:36:50
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

        void *responder = zmq_socket(context, ZMQ_REP);
        zmq_connect(responder, "tcp://localhost:5560");

        char buf[10];
        while(1) {
                zmq_recv(responder, buf, sizeof(buf), 0);
                printf("recv: %s\n", buf);

                sleep(1);

                zmq_send(responder, "World", 6, 0);
        }

        zmq_close(responder);
        zmq_ctx_destroy(context);
        return 0;
}
