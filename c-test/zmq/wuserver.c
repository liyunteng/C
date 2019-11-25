/*
 * wuserver.c -- weather update
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/25 18:36:30
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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <zmq.h>

int
main(int argc, char *argv[])
{
    void *context   = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int   rc        = zmq_bind(publisher, "tcp://*:5556");
    assert(rc == 0);
    rc = zmq_bind(publisher, "ipc://weather.ipc");
    assert(rc == 0);

    srandom(time(NULL));
    while (1) {

        int zipcode, temperature, rel;
        zipcode     = random() % 100000;
        temperature = random() % 215 - 80;
        rel         = random() % 50 + 10;

        char update[20];
        sprintf(update, "%05d %d %d", zipcode, temperature, rel);
        zmq_send(publisher, update, sizeof(update), 0);
    }

    zmq_close(publisher);
    zmq_ctx_destroy(context);
    return 0;
}
