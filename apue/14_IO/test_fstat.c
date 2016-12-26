/*
 * test_fstat.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng
 * License: GPL
 * Update time:
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int fd;
    struct stat statbuf;

    if (argc != 2) {
	fprintf(stderr, "usage: %s <filename>\n", argv[0]);
	return (-1);
    }

    if ((fd = open(argv[0], O_RDONLY)) < 0) {
	fprintf(stderr, "open file error: %s\n", strerror(errno));
	return (errno);
    }

    if (fstat(fd, &statbuf) != 0) {
	fprintf(stderr, "fstat error: %s\n", strerror(errno));
	return (errno);
    }

    printf("size: %lu\n", statbuf.st_size);
    return 0;
}
