/*
 * client.c -- client
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/27 14:50:48
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
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <zmq.h>

void *rep;
void *req;
void *
run(void *arg)
{
    int  ret;
    char buf[256];
    while (1) {
        memset(buf, 0, sizeof(buf));
        zmq_recv(rep, buf, sizeof(buf), 0);
        printf("%lu recv: %s\n", pthread_self(), buf);
        memset(buf, 0, sizeof(buf));
        ret = zmq_send(rep, "recv done", 10, 0);
        assert(ret == 10);
    }
}

int
main(void)
{
    int   ret;
    void *context = zmq_ctx_new();

    rep = zmq_socket(context, ZMQ_REP);
    req = zmq_socket(context, ZMQ_REQ);

    ret = zmq_connect(req, "tcp://localhost:2002");
    assert(ret == 0);
    ret = zmq_connect(rep, "tcp://localhost:2003");
    assert(ret == 0);

    pthread_t pid;
    if (pthread_create(&pid, NULL, run, NULL) != 0) {
        printf("create pthread failed.");
        return -1;
    }

    sleep(2);
    struct timeval tv;
    char           buf[256];

    while (1) {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "req from %lu", pthread_self());
        tv.tv_sec  = 1;
        tv.tv_usec = 0;
        select(0, NULL, NULL, NULL, &tv);
        ret = zmq_send(req, buf, sizeof(buf), 0);
        assert(ret == sizeof(buf));
        memset(buf, 0, sizeof(buf));
        zmq_recv(req, buf, sizeof(buf), 0);
        printf("req recv: %s\n", buf);
    }

    pthread_join(pid, NULL);
    return 0;
}
