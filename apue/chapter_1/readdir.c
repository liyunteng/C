/*
 * Description: ls
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/12/15 16:28:43>
 */
#include "ourhdr.h"
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>

int
main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *dirp;

    if (argc != 2) {
        err_quit("a single argument (the directory name) is required");
    }

    if ((dp = opendir(argv[1])) == NULL) {
        err_sys("can't open %s", argv[1]);
    }

    while ((dirp = readdir(dp)) != NULL) {
        printf("%s\n", dirp->d_name);
    }
}
