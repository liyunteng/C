/*
 * test_NONBLOCK.c-- ./test_NONBLOCK < /dev/sda1 2>stderr && cat stderr
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/10/30 09:54:43
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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>


char buf[500000];

void set_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
	fprintf(stderr, "fcntl F_GETFL error: %s\n", strerror(errno));
	exit(errno);
    }

    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0) {
	fprintf(stderr, "fcntl F_SETFL error: %s\n", strerror(errno));
	exit(errno);
    }
    return;
}

void clr_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
	fprintf(stderr, "fcntl F_GETFL error: %s\n", strerror(errno));
	exit(errno);
    }

    val &= ~flags;

    if (fcntl(fd, F_SETFL, val) < 0) {
	fprintf(stderr, "fcntl F_SETFL error: %s\n", strerror(errno));
	exit(errno);
    }
    return;
}

int main(int argc, char *argv[])
{
    int ntowrite, nwrite;
    char *ptr;

    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes\n", ntowrite);

    set_fl(STDOUT_FILENO, O_NONBLOCK);

    ptr = buf;
    while (ntowrite > 0) {
	errno = 0;
	nwrite = write(STDOUT_FILENO, ptr, ntowrite);
	fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

	if (nwrite > 0) {
	    ptr += nwrite;
	    ntowrite -= nwrite;
	}
    }

    clr_fl(STDOUT_FILENO, O_NONBLOCK);
    return 0;
}
