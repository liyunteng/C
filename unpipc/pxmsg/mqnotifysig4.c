/*
 * mqnotifysig4.c--
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

#include <signal.h>
#include <mqueue.h>
#include "../unpipc.h"

int main(int argc, char *argv[])
{
    int signo, err;
    mqd_t mqd;
    void *buf;
    ssize_t n;
    sigset_t newmask;
    struct mq_attr attr;
    struct sigevent sigev;

    if (argc != 2)
	err_quit("usage: mqnotifysig4 <name>\n");

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

    if (sigemptyset(&newmask) < 0) {
	err_sys("sigemptyset error: ");
    }
    if (sigaddset(&newmask, SIGUSR1) < 0) {
	err_sys("sigaddset error: ");
    }
    if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0) {
	err_sys("sigprocmask error: ");
    }

    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    if (mq_notify(mqd, &sigev) < 0) {
	err_sys("mq_notfiy error: ");
    }

    for (;;) {
	if ((err = sigwait(&newmask, &signo)) != 0) {
	    fprintf(stderr, "sigwait error: %s\n", strerror(errno));
	    exit(err);
	}
	if (signo == SIGUSR1) {
	    if (mq_notify(mqd, &sigev) < 0) {
		err_sys("mq_notify error: ");
	    }
	    while ((n = mq_receive(mqd, buf, attr.mq_msgsize, NULL)) >= 0) {
		printf("read %ld bytes\n", (long) n);
	    }
	    if (errno != EAGAIN) {
		err_sys("mq_receive error: ");
	    }
	}
    }

    return 0;
}
