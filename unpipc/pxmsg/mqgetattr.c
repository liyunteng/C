/*
 * mqgetattr.c--
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

#include <mqueue.h>
#include "../unpipc.h"

int main(int argc, char *argv[])
{
	mqd_t	mqd;
	struct mq_attr attr;

	if (argc != 2)
		err_quit("usage: mgetattr <name>\n");

	mqd = mq_open(argv[1], O_RDONLY);
	if (mqd < 0)
		err_quit("mq_open error:");

	if (mq_getattr(mqd, &attr) < 0) {
		err_quit("mq_getattr error:");
	}
	printf("flags #flags = %ld, max #msgs = %ld, max #bytes/msg = %ld, "
	       "#currently on queue = %ld\n", attr.mq_flags,
	       attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

	mq_close(mqd);
	
	return 0;
}


