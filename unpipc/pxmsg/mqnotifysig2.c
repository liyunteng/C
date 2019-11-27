/*
 * mqnotify2.c--
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
#include <signal.h>

volatile sig_atomic_t mqflag;
static void sig_usr1(int signo);

int
main(int argc, char *argv[])
{
    mqd_t mqd;
    void *buf;
    struct sigevent sigev;
    struct mq_attr attr;
    ssize_t n;
    sigset_t zeromask, newmask, oldmask;

    if (argc != 2) {
        err_quit("usage: mqnotifysig2 <name>\n");
    }

    if ((mqd = mq_open(argv[1], O_RDONLY)) < 0) {
        err_sys("mq_open error: ");
    }
    if (mq_getattr(mqd, &attr) < 0) {
        err_sys("mq_getattr error: ");
    }

    buf = malloc(attr.mq_maxmsg);
    if (buf == NULL) {
        err_sys("malloc error: ");
    }

    if (sigemptyset(&zeromask) < 0) {
        err_sys("sigemptyset error: ");
    }
    if (sigemptyset(&newmask) < 0) {
        err_sys("sigemptyset error: ");
    }
    if (sigemptyset(&oldmask) < 0) {
        err_sys("sigemptyset error: ");
    }
    if (sigaddset(&newmask, SIGUSR1) < 0) {
        err_sys("sigaddset error: ");
    }

    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        err_sys("signal error: ");
    }
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo  = SIGUSR1;
    if (mq_notify(mqd, &sigev) < 0) {
        err_sys("mq_notify error: ");
    }

    for (;;) {
        if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
            err_sys("sigprocmask error: ");
        }
        while (mqflag == 0) {
            sigsuspend(&zeromask);
        }
        mqflag = 0;

        if (mq_notify(mqd, &sigev) < 0) {
            err_sys("mq_notify error: ");
        }
        /* 如果同时有多条消息到达，会有消息丢失，改为while */
        n = mq_receive(mqd, buf, attr.mq_msgsize, NULL);
        if (n < 0) {
            err_sys("mq_receive error: ");
        }
        printf("read %ld bytes\n", (long)n);
        if (sigprocmask(SIG_UNBLOCK, &newmask, NULL) < 0) {
            err_sys("sigprocmask error: ");
        }
    }

    return 0;
}

static void
sig_usr1(int signo)
{
    mqflag = 1;
    return;
}
