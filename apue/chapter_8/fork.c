/*
 * 8-1.c - 8-1 fork
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/19
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include "ourhdr.h"
#include <sys/types.h>

int glob   = 6;
char buf[] = "a write to stdout\n";

int
main(void)
{
    int var;
    pid_t pid;

    var = 88;
    if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1) {
        err_sys("write error");
    }

    if ((pid = fork()) < 0) {
        err_sys("fork error");

    } else if (pid == 0) {
        /* child */
        glob++;
        var++;
    } else {
        /* parent */
        sleep(2);
    }

    printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);

    return 0;
}
