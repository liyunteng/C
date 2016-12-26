/*
 * test_mmap.c--
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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>


int main(int argc, char *argv[])
{
	int	fdin, fdout;
	void	*src, *dst;
	struct stat statbuf;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <fromfile> <tofile>\n", argv[0]);
		return (-1);
	}

	if ((fdin = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "can't open %s for reading: %s\n", argv[1],
			strerror(errno));
		return (errno);
	}
	
	if ((fdout = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, 644)) < 0) {
		fprintf(stderr, "can;t open %s for wirting: %s\n", argv[2],
			strerror(errno));
		return(errno);
	}
	
	if (fstat(fdin, &statbuf) != 0) {
		fprintf(stderr, "fstat error: %s\n", strerror(errno));
		return(errno);
	}
	
	/* set size of output file */
	if (lseek(fdout, statbuf.st_size-1, SEEK_SET) == -1) {
		fprintf(stderr, "lseek error: %s\n", strerror(errno));
		return (-5);
	}
	
	if (write(fdout, "", 1) != 1) {
		fprintf(stderr, "write error: %s\n", strerror(errno));
		return (-6);
	}

	if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED,
			fdin, 0)) == MAP_FAILED) {
		fprintf(stderr, "mmap error for input: %s\n", strerror(errno));
		return (-7);
	}

	if ((dst = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE,
			MAP_SHARED, fdout, 0)) == MAP_FAILED) {
		fprintf(stderr, "mmap error for output: %s\n",strerror(errno));
		return (-8);
	}
	
	memcpy(dst, src, statbuf.st_size);
		
	/* 
         * if (msync(dst, statbuf.st_size, MS_SYNC) != 0) {
	 * 	fprintf(stderr, "msync error: %s\n", strerror(errno));
	 * 	return (-10);
	 * }
	 * munmap(src, statbuf.st_size);
	 * munmap(src, statbuf.st_size);
         */
	
	return 0;
}
