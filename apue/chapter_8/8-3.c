/*
 * 8-3.c - 8-3 exit code
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/20
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */

#include "ourhdr.h"
#include <sys/types.h>
#include <sys/wait.h>

int
main(void)
{
    pid_t pid;
    int   status;

    if ((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid == 0)
        exit(7);

    if (wait(&status) != pid)
        err_sys("wait error");
    pr_exit(status);

    if ((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid == 0)
        abort();

    if (wait(&status) != pid)
        err_sys("wait error");
    pr_exit(status);

    if ((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid == 0)
        status /= 0;

    if (wait(&status) != pid)
        err_sys("wait error");
    pr_exit(status);

    return 0;
}
