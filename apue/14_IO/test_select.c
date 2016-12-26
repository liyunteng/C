/*
 * test_select.c--
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/time.h>

#define MAXLINE 1024

int main(int argc, char *argv[])
{
    int keyboard, ret;
    char c;
    fd_set readfd;
    struct timeval timeout;

    keyboard = open("/dev/tty", O_RDONLY);
    assert(keyboard > 0);

    while (1) {
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	FD_ZERO(&readfd);
	FD_SET(keyboard, &readfd);
	ret = select(keyboard + 1, &readfd, NULL, NULL, &timeout);

	if (ret == -1) {
	    perror("select error");
	} else if (ret) {
	    if (FD_ISSET(keyboard, &readfd)) {
		read(keyboard, &c, 1);
		printf("%c", c);
		if ('q' == c)
		    break;
	    }
	} else if (ret == 0) {
	    printf("time out\n");
	}

    }

    return 0;
}
