/*
 * taskvent.c -- task vent
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/26 10:23:15
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
#include <stdlib.h>
#include <time.h>

int main(void)
{
    void *context = zmq_ctx_new();

    void *sender = zmq_socket(context, ZMQ_PUSH);
    zmq_bind(sender, "tcp://*:5557");

    void *sink = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sink, "tcp://localhost:5558");

    printf("Press Enter when the workers are ready: ");
    getchar();
    printf("Sending tasks to workers...\n");


    int ret = zmq_send(sink, "0", sizeof("0"), 0);
    assert(ret == 2);

    srandom(time(NULL));
    int task_nbr;
    int total_msec = 0;
    for (task_nbr = 0; task_nbr != 100; ++task_nbr) {
	int workload;

	workload = random() % 100 + 1;
	total_msec += workload;
	char string[10];
	sprintf(string, "%d", workload);
	ret = zmq_send(sender, string, strlen(string) + 1, 0);
	assert(ret == strlen(string) + 1);
    }

    printf("Total expencted cost: %d msec", total_msec);
    sleep(1);

    zmq_close(sink);
    zmq_close(sender);
    zmq_ctx_destroy(context);
    return 0;
}
