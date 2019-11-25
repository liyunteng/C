/*
 * test1.c--
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

#include "../../unpipc.h"
#include <signal.h>

static void sig_rt(int, siginfo_t *, void *);
int         signal_rt(int, void (*)(int, siginfo_t *, void *), sigset_t *);

int
main(int argc, char *argv[])
{
    int          i, j;
    pid_t        pid;
    sigset_t     newset;
    union sigval val;

    printf("SIGRTMIN = %d, SIGRTMAX = %d\n", (int)SIGRTMIN, (int)SIGRTMAX);

    if ((pid = fork()) < 0) {
        err_quit("fork error: ");
    } else if (pid == 0) {
        /* child : block three realtime signals */
        if (sigemptyset(&newset) < 0) {
            err_sys("sigemptyset error: ");
        }
        if (sigaddset(&newset, SIGRTMAX) < 0) {
            err_sys("sigaddset SIGRTMAX error: ");
        }
        if (sigaddset(&newset, SIGRTMAX - 1) < 0) {
            err_sys("sigaddset SIGRTMAX-1 error: ");
        }
        if (sigaddset(&newset, SIGRTMAX - 2) < 0) {
            err_sys("sigaddset SIGERMAX-2 error: ");
        }
        if (sigprocmask(SIG_BLOCK, &newset, NULL) < 0) {
            err_sys("sigprocmask error: ");
        }

        /* 非realtime信号，指定sigaction中的SA_SIGINFO结果是未
         * 定义的。 在linux上测试时， 只发送一次信号，并且在
         * realtime信号delievered之前 */
        if (signal_rt(SIGUSR1, sig_rt, &newset) < 0) {
            err_sys("signal_rt SIGUSR1 error: ");
        }
        if (signal_rt(SIGRTMAX, sig_rt, &newset) < 0) {
            err_sys("signal_rt SIGRTMAX error: ");
        }
        if (signal_rt(SIGRTMAX - 1, sig_rt, &newset) < 0) {
            err_sys("signal_rt SIGRTMAX-1 error: ");
        }
        if (signal_rt(SIGRTMAX - 2, sig_rt, &newset) < 0) {
            err_sys("signal_rt SIGRTMAX-2 error: ");
        }

        sleep(6); /* let parent send all signals */

        if (sigprocmask(SIG_UNBLOCK, &newset, NULL) < 0) {
            err_sys("sigprocmask error: ");
        }
        sleep(3); /* let all queued signals be delivered */
        exit(0);
    }

    /* parent: sends nine signals to child */
    sleep(3); /* let child block all signals */
    for (i = SIGRTMAX; i >= SIGRTMAX - 2; i--) {
        for (j = 0; j <= 2; j++) {
            val.sival_int = j;
            if (sigqueue(pid, i, val) < 0) {
                err_sys("sigqueue error: ");
            }
            printf("send signal %d, val = %d\n", i, j);
        }
    }
    for (j = 0; j <= 2; j++) {
        val.sival_int = j;
        if (sigqueue(pid, SIGUSR1, val) < 0) {
            err_sys("sigqueue error: ");
        }
        printf("send signal %d, val = %d\n", SIGUSR1, j);
    }

    return 0;
}

static void
sig_rt(int signo, siginfo_t *info, void *context)
{
    printf("received signal #%d, code = %d, ival = %d\n", signo, info->si_code,
           info->_sifields._rt.si_sigval.sival_int);
}
