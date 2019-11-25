/*
 * 8-8.c - 8-8 exec
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/21
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */

#include "ourhdr.h"
#include <sys/types.h>
#include <sys/wait.h>

char *env_init[] = {"USER=unknown", "PATH=/tmp", NULL};

int
main(void)
{
    pid_t pid;

    if ((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid == 0) {
        if (execle("/bin/ls", "ls",
                   "/"
                   "/home/lyt",
                   (char *)0, env_init)
            < 0)
            err_sys("execle error");
    }

    if (waitpid(pid, NULL, 0) < 0)
        err_sys("wait error");

    if ((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid == 0) {
        if (execlp("ls", "ls", "/home/lyt", (char *)0) < 0)
            err_sys("execlp error");
    }
    return 0;
}
