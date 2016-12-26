/*
 * lock_test.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/11/12 14:59:44
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
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    if (fcntl(fd, F_GETLK, &lock) < 0) {
	fprintf(stderr, "fcntl error: %s\n", strerror(errno));
	exit(-1);
    }

    if (lock.l_type == F_UNLCK)
	return (0);
    return (lock.l_pid);
}

int main(int argc, char *argv[])
{
    int fd;
    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    /* 注意是5604 而不是2604 g+s,g-x, 设置T，S位的时候取掩码 */
    /* 需要挂载时，指定 -o mand选项 */
    if ((fd = open("./tmplock", O_RDWR | O_CREAT, 5604)) < 0) {
	fprintf(stderr, "open file error: %s\n", strerror(errno));
	return (errno);
    }
    if (lock_test(fd, F_WRLCK, 0, SEEK_SET, 0) != 0) {
	fprintf(stderr, "lock_test faile\n");
	return (errno);
    }

    if (fcntl(fd, F_SETLK, &lock) != 0) {
	fprintf(stderr, "lock file error: %s\n", strerror(errno));
	return (errno);
    }

    pause();

    return 0;
}
