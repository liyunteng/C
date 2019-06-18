/*
 * 5-2.c - copy from stdin to stdout by fgets fputs
 *
 * Author : liyunteng <li_yunteng@163.com>
 * Date   : 2019/06/18
 */
#include "ourhdr.h"

int main(void)
{
    char buf[MAXLINE];

    while (fgets(buf, MAXLINE, stdin) != NULL) {
        if (fputs(buf, stdout) == EOF) {
            err_sys("output error");
        }
    }

    if (ferror(stdin)) {
        err_sys("input error");
    }
    return 0;
}
