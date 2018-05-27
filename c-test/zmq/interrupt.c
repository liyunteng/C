/*
 * interrupt.c -- show how to handle Ctrl-C
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/28 13:06:27
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
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <signal.h>

static int s_interrupted = 0;

static void s_signal_handler(int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals(void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}


int main(void)
{
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REP);
    zmq_bind(socket, "tcp://*:5555");

    s_catch_signals();
    while (1) {
	zmq_msg_t message;
	zmq_msg_init(&message);
	zmq_msg_recv(&message, socket, 0);

	if (s_interrupted) {
	    printf("W: interrupt recieved. killing server...\n");
	    break;
	}
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);
    return 0;
}
