/*
 * Description: 4_5 unlink
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/13 22:41:58 liyunteng>
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ourhdr.h"

int main(void)
{
    if (open("tempfile", O_RDWR | O_CREAT, FILE_MODE) < 0)
        err_sys("open error");

    sleep(10);
    if (unlink("tempfile") < 0)
        err_sys("unlink error");

    printf("file unlinked\n");
    sleep(10);
    printf("done\n");

    return 0;
}
