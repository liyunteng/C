/*
 * 9-1.c - 9-1 orphan process group
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/22
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include "ourhdr.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

static void sig_tt(int);
static void sig_hup(int);
static void pr_ids(char *);

int
main(void)
{
    char c;
    pid_t pid;

    pr_ids("parent");

    if ((pid = fork()) < 0)
        err_sys("fork error");

    else if (pid > 0) {
        sleep(5);
        printf("parent exit\n");
        exit(0);
    } else {
        pr_ids("child");
        signal(SIGHUP, sig_hup);
        signal(SIGTTIN, sig_tt);
        signal(SIGTTOU, sig_tt);
        signal(SIGCONT, sig_tt);
        kill(getpid(), SIGTSTP);
        pr_ids("child");
        if (read(STDIN_FILENO, &c, 1) != 1) {
            printf("read error from control terminal, errno = %d(%s)\n", errno, strerror(errno));
        } else {
            printf("read %d(%c)\n", c, c);
        }
        exit(0);
    }
    return 0;
}

static void
sig_tt(int signo)
{
    printf("%s received, pid = %d\n", strsignal(signo), getpid());
    return;
}

static void
sig_hup(int signo)
{
    printf("SIGHUP received, pid = %d\n", getpid());
    return;
}

static void
pr_ids(char *name)
{
    printf("%s: pid = %d, ppid = %d, pgrp = %d, sid = %d\n", name, getpid(), getppid(),
           getpgrp(), getsid(getpid()));
    fflush(stdout);
}
