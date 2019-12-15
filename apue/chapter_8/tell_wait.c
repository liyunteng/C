/*
 * 8-6.c - 8-6
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/21
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include "ourhdr.h"
#include <sys/types.h>

static void charatatime(char *);

int
main(void)
{
    pid_t pid;

    TELL_WAIT();

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        WAIT_PARENT();
        charatatime("output from child\n");
    } else {
        charatatime("output from parent\n");
        TELL_CHILD(pid);
    }
    return 0;
}

static void
charatatime(char *str)
{
    char *ptr;
    char c;

    setbuf(stdout, NULL);
    for (ptr = str; (c = *ptr++);)
        putc(c, stdout);
}
