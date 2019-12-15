/*
 * Description: 1-6
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/12/15 16:30:18>
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
