/*
 * 7-2.c - arguments
 *
 * Author : liyunteng <li_yunteng@163.com>
 * Date   : 2019/06/19
 */

#include "ourhdr.h"

int
main(int argc, char *argv[])
{
    int i;
    for (i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    return 0;
}
