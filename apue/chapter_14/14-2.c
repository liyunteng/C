/*
 * 14-2.c - 14-2
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/24
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include "ourhdr.h"
#include <sys/wait.h>

#define DEF_PAGER "/usr/bin/more"

int
main(int argc, char *argv[])
{
    int   n, fd[2];
    pid_t pid;
    char  line[MAXLINE], *pager, *argv0;
    FILE *fp;

    if (argc != 2)
        err_quit("usage: %s <pathname>", argv[0]);

    if ((fp = fopen(argv[1], "r")) == NULL)
        err_sys("can't open %s", argv[1]);
    if (pipe(fd) < 0)
        err_sys("pipe error");

    if ((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid > 0) {
        close(fd[0]);
        while (fgets(line, MAXLINE, fp) != NULL) {
            n = strlen(line);
            if (write(fd[1], line, n) != n)
                err_sys("write error to pipe");
        }

        if (ferror(fp))
            err_sys("fgets error");

        close(fd[1]);
        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");
        exit(0);
    } else {
        close(fd[1]);
        if (fd[0] != STDIN_FILENO) {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
                err_sys("dup2 error to stdin");
            close(fd[0]);
        }

        if ((pager = getenv("PAGER")) == NULL)
            pager = DEF_PAGER;
        if ((argv0 = strrchr(pager, '/')) != NULL)
            argv0++;
        else
            argv0 = pager;

        if (execl(pager, argv0, (char *)0) < 0)
            err_sys("execl error for %s", pager);
    }
    return 0;
}
