/*
 * 10-2.c - 10-2
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/22
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */

#include "ourhdr.h"
#include <pwd.h>
#include <signal.h>

static void my_alarm(int);

int
main(void)
{
    struct passwd *ptr;
    signal(SIGALRM, my_alarm);
    alarm(1);

    for (;;) {
        if ((ptr = getpwnam("lyt")) == NULL)
            err_sys("getpwnam error");
        if (strcmp(ptr->pw_name, "lyt") != 0)
            printf("return value corrupted!, pw_name=%s\n", ptr->pw_name);
    }
    return 0;
}

static void
my_alarm(int signo)
{
    struct passwd *rootptr;

    printf("in signal handler\n");
    if ((rootptr = getpwnam("root")) == NULL)
        err_sys("getpwnam(root) error");
    alarm(1);
    return;
}
