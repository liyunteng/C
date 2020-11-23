/*
 * Description: 1-3 buffered io
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/12/15 16:29:30>
 */

#include "ourhdr.h"

int
main(void)
{
    int c;

    while ((c = getc(stdin)) != EOF) {
        if (putc(c, stdout) == EOF) {
            err_sys("output error");
        }
    }
    if (ferror(stdin)) {
        err_sys("input error");
    }
    return 0;
}
