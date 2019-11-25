/*
 * 10-18.c - 10-18 abort() posix
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/23
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>

#include "ourhdr.h"

void
abort_1(void)
{
    sigset_t         mask;
    struct sigaction action;
    sigaction(SIGABRT, NULL, &action);
    if (action.sa_handler == SIG_IGN) {
        action.sa_handler = SIG_DFL;
        sigaction(SIGABRT, &action, NULL);
    }

    if (action.sa_handler == SIG_DFL)
        fflush(NULL);

    sigfillset(&mask);
    sigdelset(&mask, SIGABRT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    kill(getpid(), SIGABRT);

    fflush(NULL);

    action.sa_handler = SIG_DFL;
    sigaction(SIGABRT, &action, NULL);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    kill(getpid(), SIGABRT);

    exit(1);
}

int
main(void)
{
    abort_1();
    return 0;
}
