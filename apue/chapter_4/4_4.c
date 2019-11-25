/*
 * Description: 4_4
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/13 22:27:48 liyunteng>
 */
#include "ourhdr.h"
#include <sys/stat.h>
#include <sys/types.h>

int
main(void)
{
    struct stat statbuf;

    if (stat("foo", &statbuf) < 0)
        err_sys("stat error for foo");
    if (chmod("foo", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
        err_sys("chmod error for foo");

    if (chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)
        err_sys("chmod error for bar");

    return 0;
}
