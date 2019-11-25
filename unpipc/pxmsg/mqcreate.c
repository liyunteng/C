/*
 * mqcreate.c--
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

struct mq_attr attr;

int
main(int argc, char *argv[])
{
    int   c, flags;
    mqd_t mqd;

    flags = O_RDWR | O_CREAT;
    while ((c = getopt(argc, argv, "em:z:")) != -1) {
        switch (c) {
        case 'e':
            flags |= O_EXCL;
            break;
        case 'm':
            attr.mq_maxmsg = atol(optarg);
            break;
        case 'z':
            attr.mq_msgsize = atol(optarg);
            break;
        default:
            exit(1);
        }
    }
    if (optind != argc - 1)
        err_quit("usage: mqcreate [ -e ] [ -m maxmsg -z msgsize ] <name>\n");

    if ((attr.mq_maxmsg != 0 && attr.mq_msgsize == 0)
        || (attr.mq_maxmsg == 0 && attr.mq_msgsize != 0))
        err_quit("must specify both -m maxmsg and -z msgsize\n");

    if ((mqd = mq_open(argv[optind], flags, 644, (attr.mq_maxmsg != 0) ? &attr : NULL)) < 0)
        err_sys("mq_open error:");

    mq_close(mqd);
    return 0;
}
