/*
 * mqnotifythread1.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng
 * License: GPL
 * Update time:
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

#include "../unpipc.h"
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>

mqd_t mqd;
struct mq_attr attr;
struct sigevent sigev;

static void notify_thread(sigval_t); /* our thread function */

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        err_quit("usage: mqnotifythread1 <name>\n");
    }

    if ((mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK)) < 0) {
        err_sys("mq_open error: ");
    }
    if (mq_getattr(mqd, &attr) < 0) {
        err_sys("mq_getattr error: ");
    }

    sigev.sigev_notify                       = SIGEV_THREAD;
    sigev._sigev_un._sigev_thread._function  = notify_thread;
    sigev._sigev_un._sigev_thread._attribute = NULL;
    sigev.sigev_value.sival_ptr              = NULL;
    if (mq_notify(mqd, &sigev) < 0) {
        err_sys("mq_notify error: ");
    }

    for (;;)
        pause();
    return 0;
}

static void
notify_thread(sigval_t sigval)
{
    void *buf;
    ssize_t n;

    printf("notify_thread start...\n");
    buf = malloc(attr.mq_msgsize);
    if (buf == NULL) {
        err_sys("malloc error: ");
    }

    /* 不要忘记要重新注册 */
    if (mq_notify(mqd, &sigev) < 0) {
        err_sys("mq_notify error: ");
    }

    while ((n = mq_receive(mqd, buf, attr.mq_msgsize, NULL)) >= 0) {
        printf("read %ld bytes\n", (long)n);
    }
    if (errno != EAGAIN) {
        err_sys("mq_receive error: ");
    }

    free(buf);
    pthread_exit(NULL);
}
