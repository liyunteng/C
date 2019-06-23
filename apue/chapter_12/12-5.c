/*
 * 12-5.c - 12-5 sigle daemon
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/23
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "ourhdr.h"

#define PIDFILE "daemon.pid"

int main(void)
{
    int fd, val;
    char buf[10];

    if ((fd = open(PIDFILE, O_WRONLY | O_CREAT, FILE_MODE)) < 0)
        err_sys("open error");

    if (write_lock(fd, 0, SEEK_SET, 0) < 0) {
        if (errno == EACCES || errno == EAGAIN)
            exit(0);
        else
            err_sys("write_lock error");
    }

    if (ftruncate(fd,0) < 0)
        err_sys("ftruncate error");

    sprintf(buf, "%d\n", getpid());
    if (write(fd, buf, strlen(buf)) != strlen(buf))
        err_sys("write error");

    if ((val = fcntl(fd, F_GETFD, 0)) < 0) {
        err_sys("fcntl F_GETFD error");
    }
    val |= FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, val) < 0) {
        err_sys("fcntl F_SETFD error");
    }

    /* xxxxx */
    sleep(10);

    exit(0);
}
