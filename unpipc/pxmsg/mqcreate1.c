/*
 * mqcreatel.c--
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


#include "../unpipc.h"
#include <mqueue.h>

int main(int argc, char *argv[])
{
    int c, flags;
    mqd_t mqd;

    flags = O_RDWR | O_CREAT;
    while ((c = getopt(argc, argv, "e")) != -1) {
	switch (c) {
	case 'e':
	    flags |= O_EXCL;
	    break;
	}
    }

    if (optind != argc - 1) {
	err_quit("usage: mqcreatel [ -e  <name> ]\n");
    }

    if ((mqd = mq_open(argv[optind], flags, 644, NULL)) < 0) {
	err_quit("mq_open fialed:");
    }
    mq_close(mqd);

    return 0;
}
