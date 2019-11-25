/*
 * 10-6.c - 10-6 sleep
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/23
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include "ourhdr.h"
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

static jmp_buf env_alrm;

static void
sig_alrm(int signo)
{
    longjmp(env_alrm, 1);
}

unsigned int
sleep2(unsigned int nsecs)
{
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        return (nsecs);

    if (setjmp(env_alrm) == 0) {
        alarm(nsecs);
        pause();
    }

    return (alarm(0));
}

static void
sig_int(int signo)
{
    int          i;
    volatile int j;

    printf("\n sig_int stating\n");
    for (i = 0; i < 2000000; i++) {
        j += i * i;
    }
    printf("sig_int finished\n");
    return;
}

int
main(void)
{
    unsigned int unslept;

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");

    unslept = sleep2(5);
    printf("sleep2 returned: %u\n", unslept);
    return 0;
}
