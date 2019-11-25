/*
 * 12-6.c - 12-6
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/24
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */

#include "ourhdr.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int
main(void)
{
    int i, fd;

    if ((fd = open("temp.lock", O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0)
        err_sys("open error");

    for (i = 0; i < 100000; i++) {
        if (writew_lock(fd, 0, SEEK_END, 0) < 0)
            err_sys("writew_lock error");

        if (write(fd, &fd, 1) != 1)
            err_sys("write error");

        if (un_lock(fd, 0, SEEK_END, 0) < 0)
            err_sys("un_lock error");

        if (write(fd, &fd, 1) != 1)
            err_sys("write error");
    }
    return 0;
}
