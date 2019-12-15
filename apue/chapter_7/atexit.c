/*
 * 7-1.c - atexit only normal exit
 *
 * Author : liyunteng <li_yunteng@163.com>
 * Date   : 2019/06/19
 */

#include "ourhdr.h"

static void my_exit1(void), my_exit2(void);

int
main(void)
{
    if (atexit(my_exit2) != 0) {
        err_sys("can't register my_exit2");
    }
    if (atexit(my_exit1) != 0) {
        err_sys("can't register my_exit1");
    }
    if (atexit(my_exit1) != 0) {
        err_sys("can't register my_exit1");
    }
    /* char *a = NULL;
     * sprintf(a, "abc"); */
    /* abort(); */
    printf("main is done\n");
    return 0;
}

static void
my_exit1(void)
{
    printf("first exit handler\n");
}

static void
my_exit2(void)
{
    printf("second exit handler\n");
}
