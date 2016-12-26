/*
 * hwclient.c -- hello world client
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/05/28 11:10:49
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

int main(int argc, char *argv[])
{
    printf("Connecting to hello world server...\n");
    void *context = zmq_ctx_new();
    void *request = zmq_socket(context, ZMQ_REQ);

    zmq_connect(request, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
	char buffer[10];
	printf("Sending Hello %d...\n", request_nbr);
	zmq_send(request, buffer, sizeof(buffer) + 1, 0);
	zmq_recv(request, buffer, 10, 0);
	printf("Received %s %d\n", buffer, request_nbr);
    }

    zmq_close(request);
    zmq_ctx_destroy(context);
    return 0;
}
