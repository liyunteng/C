/*
 * Description: 3-3 test buffsize
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/12 00:57:19 liyunteng>
 */
#include "ourhdr.h"

#define BUFFSIZE 8192

int
main(void)
{
    int  n;
    char buf[BUFFSIZE];

    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
        if (write(STDOUT_FILENO, buf, n) != n)
            err_sys("write error");

    if (n < 0) {
        err_sys("read error");
    }
    return 0;
}
