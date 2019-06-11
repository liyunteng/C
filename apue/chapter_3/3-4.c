/*
 * Description: 3-4 test fctnl F_GETFL
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/12 01:04:06 liyunteng>
 */
#include <sys/types.h>
#include <fcntl.h>
#include "ourhdr.h"

int main(int argc, char *argv[])
{
    int accmode, val;

    if (argc != 2)
        err_quit("usage: %s <descriptor#", argv[0]);

    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
        err_sys("fcntl error for fd %d", atoi(argv[1]));

    accmode = val & O_ACCMODE;
    if      (accmode == O_RDONLY) printf("read only");
    else if (accmode == O_WRONLY) printf("write only");
    else if (accmode == O_RDWR)   printf("read write");

    if (val & O_APPEND)           printf(", append");
    if (val & O_NONBLOCK)         printf(", nonblocking");
#if !defined(_POSIX_SOURCE) && defined(O_SYNC)
    if (val & O_SYNC)             printf(", synchronous writes");
#endif

    putchar('\n');
    return 0;
}
