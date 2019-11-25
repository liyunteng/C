/*
 * Description: 1-6
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/11 22:13:41 liyunteng>
 */
#include "ourhdr.h"
#include <errno.h>

int
main(int argc, char *argv[])
{
    fprintf(stderr, "EACCES: %s\n", strerror(EACCES));

    errno = ENOENT;
    perror(argv[0]);
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o 1-6 1-6.c -g -I../include -L../lib -lourhdr" */
/* End: */
