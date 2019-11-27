/*
 * 5-5.c - use tmpfile
 *
 * Author : liyunteng <li_yunteng@163.com>
 * Date   : 2019/06/18
 */

#include "ourhdr.h"

int
main(int argc, char *argv[])
{
    if (argc != 3)
        err_quit("usage: %s <directory> <prefix>", argv[0]);

    printf("%s\n", tempnam(argv[1][0] != ' ' ? argv[1] : NULL,
                           argv[2][0] != ' ' ? argv[2] : NULL));
    return 0;
}
