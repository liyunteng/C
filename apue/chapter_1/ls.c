/*
 * Description: ls
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/08 14:44:01 liyunteng>
 */
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "ourhdr.h"

int main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *dirp;

    if (argc != 2) {
        err_quit("a single argument (the directory name) is required");
    }

    if ( (dp = opendir(argv[1])) == NULL) {
        err_sys("can't open %s", argv[1]);
    }

    while ( (dirp = readdir(dp)) != NULL) {
        printf("%s\n", dirp->d_name);
    }


/* Local Variables: */
/* compile-command: "clang -Wall -g -o ls ls.c -I../include -L../lib -lourhdr" */
/* End: */
