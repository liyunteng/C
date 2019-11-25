/*
 * mqnotifysig5.c--
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
#include "libmq.h"
#include <signal.h>

static void sig_usr1(int);
static int  fd[2];

int
main(int argc, char *argv[])
{
    int             c, n, err;
    mqd_t           mqd;
    void *          buf;
    fd_set          rfd;
    struct mq_attr  attr;
    struct sigevent sigev;

    if (argc != 2) {
        err_quit("usage: mqnotifysig5 <name>\n");
    }
    if ((mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK)) < 0) {
        err_sys("mq_open error: ");
    }
    if (mq_getattr(mqd, &attr) < 0) {
        err_sys("mq_getattr error: ");
    }
    buf = malloc(attr.mq_msgsize);
    if (buf == NULL) {
        err_sys("malloc error: ");
    }

    if (pipe(fd) < 0) {
        err_sys("pipe error: ");
    }

    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        err_sys("signal error: ");
    }

    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo  = SIGUSR1;
    if (mq_notify(mqd, &sigev) < 0) {
        err_sys("mq_notify error: ");
    }

    FD_ZERO(&rfd);
    for (;;) {
        FD_SET(fd[0], &rfd);
        /* 不能根据是否小于0来进行判断，系统调用被中断后，返
         * 回-1, errno赋值为EINTR,此时并没有错误。 */
        err = select(fd[0] + 1, &rfd, NULL, NULL, NULL);
        if (err < 0 && errno != EINTR) {
            err_sys("select error: ");
        }

        if (FD_ISSET(fd[0], &rfd)) {
            if (read(fd[0], &c, 1) != 1) {
                err_sys("read error: ");
            }
            if (mq_notify(mqd, &sigev) < 0) {
                err_sys("mq_notfiy error: ");
            }
            while ((n = mq_receive(mqd, buf, attr.mq_msgsize, NULL)) >= 0) {
                printf("read %ld bytes\n", (long)n);
            }
            if (errno != EAGAIN) {
                err_sys("mq_receive error: ");
            }
        }
    }
    return 0;
}

static void
sig_usr1(int signo)
{
    if (write(fd[1], "", 1) != 1) {
        err_sys("write error: ");
    }
    return;
}
