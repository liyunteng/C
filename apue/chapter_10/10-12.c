/*
 * 10-12.c - 10-12 signal by sigaction
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/23
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */

#include <signal.h>
#include "ourhdr.h"

Sigfunc *signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }

    if (sigaction(signo, &act, &oact) < 0)
        return (SIG_ERR);
    return (oact.sa_handler);
}

Sigfunc *signal_intr(int signo, Sigfunc *func)
{
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif

    if (sigaction(signo, &act, &oact) < 0)
        return(SIG_ERR);
    return (oact.sa_handler);
}

int main(void)
{
    return 0;
}
