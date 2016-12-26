/*
 * readn_writen.c--
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
#include <stdlib.h>

ssize_t readn(int fd, void *ptr, size_t n) /* Read "n" bytes from a
					  * descriptor */
{
	size_t	nleft;
	ssize_t nread;

	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (nleft == n)
				return (-1); /* error, return -1 */
			else
				break; /* error, return amount read
					* so far */
		} else if (nread == 0) {
			break;	/* EOF */
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n-nleft); 	/* return >= 0 */
}

ssize_t writen(int fd, const void *ptr, size_t n) /* Write "n" bytes to
						 * a descriptor */
{
	size_t	nleft;
	ssize_t nwritten;

	nleft = n;
	while (nleft > 0){
		if ((nwritten = write(fd, ptr, nleft)) < 0){
			if (nleft == n)
				return(-1);
			else
				break;
		} else if (nwritten == 0) {
			break;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n - nleft);
	
}
