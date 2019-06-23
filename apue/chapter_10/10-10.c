/*
 * 10-10.c - 10-10 pr_mask
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/23
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */

#include <errno.h>
#include <signal.h>
#include "ourhdr.h"

void pr_mask(const char *str)
{
    sigset_t sigset;
    int errno_save;

    errno_save = errno;
    if (sigprocmask(0, NULL, &sigset) < 0)
        err_sys("sigprocmask error");

    printf("%s", str);
    if (sigismember(&sigset,SIGINT))    printf("SIGINT ");
    if (sigismember(&sigset, SIGQUIT))  printf("SIGQUIT ");
    if (sigismember(&sigset,SIGUSR1))   printf("SIGUSR1 ");
    if (sigismember(&sigset, SIGALRM))  printf("SIGALRM");

    printf("\n");
    errno = errno_save;
}

int main(void)
{
    pr_mask("abc");
    return 0;
}
