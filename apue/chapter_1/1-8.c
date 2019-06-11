/*
 * Description: 1-8 signal
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/11 22:20:25 liyunteng>
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "ourhdr.h"

static void sig_int(int);       /* our signal-catching function */

int main(void)
{
    char buf[MAXLINE];
    pid_t pid;
    int status;

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal error");

    printf("%% ");
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        buf[strlen(buf) - 1] = 0;

        if ((pid = fork()) < 0)
            err_sys("fork error");

        else if (pid == 0) {
            execlp(buf, buf, (char *)0);
            err_ret("couldn't execute: %s", buf);
            exit(127);
        }

        if ( (pid = waitpid(pid, &status, 0)) < 0)
            err_sys("waitpid, error");

        printf("%% ");
    }
    return 0;
}

void sig_int(int signo)
{
    printf("interrupt\n%% ");
}


/* Local Variables: */
/* compile-command: "clang -Wall -o 1-8 1-8.c -g -I../include -L../lib -lourhdr" */
/* End: */
