/*
 * Description: 1-7 print UID and GID
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/11 22:15:56 liyunteng>
 */

#include "ourhdr.h"

int
main(void)
{
    printf("uid = %d, gid = %d\n", getuid(), getgid());
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o 1-7 1-7.c -g -I../include -L../lib -lourhdr"
 */
/* End: */
