/*
 * Description: 3-5 set fl with fcntl
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/12 01:05:54 liyunteng>
 */

#include <fcntl.h>
#include "ourhdr.h"

void set_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");

    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl F_SETFL error");

}

int main(void)
{
    set_fl(0, O_RDWR);
    return 0;
}
