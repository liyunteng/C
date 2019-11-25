/*
 * 10-3.c - 10-3 SIGCLD wrong in SRV2
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/23
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

static void sig_cld(int);

int
main(void)
{
    pid_t pid;

    if (signal(SIGCLD, sig_cld) == SIG_ERR) {
        perror("signal error");
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
    } else if (pid == 0) {
        sleep(2);
        _exit(0);
    }

    pause();
    return 0;
}

static void
sig_cld(int signo)
{
    pid_t pid;
    int   status;

    printf("SIGCLD received\n");
    if (signal(SIGCLD, sig_cld) == SIG_ERR) {
        perror("signal error");
    }

    if ((pid = wait(&status)) < 0) {
        perror("wait error");
    }
    printf("pid = %d\n", pid);
    return;
}
