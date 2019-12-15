/*
 * 12-14.c - 12-14 mmap
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/24
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */

#include "ourhdr.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef MAP_FILE
#    define MAP_FILE 0
#endif

int
main(int argc, char *argv[])
{
    int fdin, fdout;
    char *src, *dst;
    struct stat statbuf;

    if (argc != 3)
        err_quit("usage: %s <fromfile> <tofile>", argv[0]);

    if ((fdin = open(argv[1], O_RDONLY)) < 0)
        err_sys("can't open %s for reading", argv[1]);

    if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0)
        err_sys("can't create %s for writing", argv[2]);

    if (fstat(fdin, &statbuf) < 0)
        err_sys("fstat error");

    if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1)
        err_sys("lseek error");
    if (write(fdout, "", 1) != 1)
        err_sys("write error");

    if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_FILE | MAP_SHARED, fdin,
                    0))
        == (caddr_t)-1)
        err_sys("mmap error for input");

    if ((dst = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE,
                    MAP_FILE | MAP_SHARED, fdout, 0))
        == (caddr_t)-1)
        err_sys("mmap error for output");

    memcpy(dst, src, statbuf.st_size);

    return 0;
}
