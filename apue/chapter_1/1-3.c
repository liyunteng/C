/*
 * Description: 1-3 buffered io
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/11 00:04:24 liyunteng>
 */

#include "ourhdr.h"

int main(void)
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

/* Local Variables: */
/* compile-command: "clang -Wall -o 1-3 1-3.c -g -I../include -L../lib -lourhdr" */
/* End: */
