/*
 * 10-19.c - 10-19 system
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/23
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include <sys/types.h>
#include <signal.h>
#include "ourhdr.h"

static void sig_int(int), sig_chld(int);

int main(void)
{
    int status;

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");
    if (signal(SIGCHLD, sig_chld) == SIG_ERR)
        err_sys("signal(SIGCHLD) error");

    if ((status = system("/usr/bin/vim")) < 0)
        err_sys("system() error");
    return 0;
}

static void sig_int(int signo)
{
    printf("caught SIGINT\n");
    return;
}

static void sig_chld(int signo)
{
    printf("caught SIGCHLD\n");
    return;
}
