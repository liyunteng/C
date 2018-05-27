/*
 * msgqueue.c -- simple message queuing broker
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/28 10:12:28
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
#include <string.h>
#include <assert.h>


int main(void)
{
    void *context = zmq_ctx_new();

    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    int rc = zmq_bind(frontend, "tcp://*:5559");
    assert(rc == 0);

    void *backend = zmq_socket(context, ZMQ_DEALER);
    rc = zmq_bind(backend, "tcp://*:5560");
    assert(rc == 0);

    zmq_proxy(frontend, backend, NULL);

    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
    return 0;
}
