/*
 * tasksink.c -- task sink
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/05/28 13:29:18
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

#include "zhelpers.h"

int main(int argc, char *argv[])
{
    void *context = zmq_ctx_new();
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_bind(receiver, "tcp://*:5558");

    char *string = s_recv(receiver);
    free(string);

    int64_t start_time = s_clock();

    int task_nbr;
    for (task_nbr = 0; task_nbr < 100; task_nbr++) {
	char *string = s_recv(receiver);
	free(string);
	if ((task_nbr / 10) * 10 == task_nbr)
	    printf(":");
	else
	    printf(".");
	fflush(stdout);
    }

    printf("Total elapsed time: %d msec\n",
	   (int) (s_clock() - start_time));

    zmq_close(receiver);
    zmq_ctx_destroy(context);

    return 0;
}
