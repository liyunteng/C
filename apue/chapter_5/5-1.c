/*
 * 5-1.c - copy from stdin to stdout by getc putc
 *
 * Author : liyunteng <li_yunteng@163.com>
 * Date   : 2019/06/18
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
