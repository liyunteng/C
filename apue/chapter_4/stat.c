/*
 * Description: 4_4
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/12/15 16:48:03>
 */
#include "ourhdr.h"
#include <sys/stat.h>
#include <sys/types.h>

int
main(void)
{
    struct stat statbuf;
    FILE *fp = NULL;
    if ((fp = fopen("foo", "a+")) == NULL) {
        err_sys("create foo failed");
    }
    if (stat("foo", &statbuf) < 0)
        err_sys("stat error for foo");

    if (chmod("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)
        err_sys("chmod error for bar");

    if (chmod("foo", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
        err_sys("chmod error for foo");

    fclose(fp);
    unlink("foo");

    return 0;
}
