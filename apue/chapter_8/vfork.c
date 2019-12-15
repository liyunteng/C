/*
 * 8-2.c - 8-2 vfork
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/19
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include "ourhdr.h"
#include <sys/types.h>

int glob = 6;

int
main(void)
{
    int var;
    pid_t pid;

    var = 88;
    printf("before vfork\n");

    if ((pid = vfork()) < 0) {
        err_sys("vfork error");
    } else if (pid == 0) {
        glob++;
        var++;
        /* _exit(0); */
        exit(0);
    }

    printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
    return 0;
}
