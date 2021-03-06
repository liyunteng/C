/*
 * Description: 1-5
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/12/15 16:30:03>
 */

#include "ourhdr.h"
#include <sys/types.h>
#include <sys/wait.h>

int
main(void)
{
    char buf[MAXLINE];
    pid_t pid;
    int status;

    printf("%% ");
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        buf[strlen(buf) - 1] = 0;

        if ((pid = fork()) < 0) {
            err_sys("fork error");

        } else if (pid == 0) { /* child */
            execlp(buf, buf, (char *)0);
            err_ret("couldn't execute: %s", buf);
            exit(127);
        }

        if ((pid = waitpid(pid, &status, 0)) < 0)
            err_sys("waitpid error");
        printf("%% ");
    }
    return 0;
}
