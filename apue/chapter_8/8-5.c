/*
 * 8-5.c - 8-5 fork twice to avoid zoombia
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/20
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include <sys/types.h>
#include <sys/wait.h>
#include "ourhdr.h"

int main(void)
{
    pid_t pid;

    if((pid = fork()) < 0)  {
        err_sys("fork error");
    } else if (pid == 0) {
        if ((pid = fork()) < 0) {
            err_sys("fork error");
        } else if (pid > 0) {
            exit(0);
        }

        sleep(2);
        printf("second child, parent pid = %d\n", getppid());
        exit(0);
    }

    if (waitpid(pid, NULL, 0) != 0) {
        err_sys("waitpid error");
    }
    return 0;
}
