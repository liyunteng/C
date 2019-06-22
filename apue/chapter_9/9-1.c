/*
 * 9-1.c - 9-1 orphan process group
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/22
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "ourhdr.h"

static void sig_hup(int);
static void pr_ids(char *);

int main(void)
{
    char c;
    pid_t pid;

    pr_ids("parent");

    if ((pid = fork()) < 0)
        err_sys("fork error");

    else if (pid > 0) {
        sleep(5);
        exit(0);
    } else {
        pr_ids("child");
        signal(SIGHUP, sig_hup);
        kill(getpid(), SIGTSTP);
        pr_ids("child");
        if (read(0, &c, 1) != 1)
            printf("read error from control terminal, errno = %d\n", errno);
        exit(0);
    }
    return 0;
}

static void sig_hup(int signo)
{
    printf("SIGHUP received, pid = %d\n", getpid());
    return;
}

static void pr_ids(char *name)
{
    printf("%s: pid = %d, ppid = %d, pgrp = %d\n",
           name, getpid(), getppid(), getpgrp());
    fflush(stdout);
}
