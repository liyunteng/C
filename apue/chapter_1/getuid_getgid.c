/*
 * Description: 1-7 print UID and GID
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/12/15 16:30:34>
 */

#include "ourhdr.h"

int
main(void)
{
    printf("uid = %d, gid = %d\n", getuid(), getgid());
    return 0;
}
