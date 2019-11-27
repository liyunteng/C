/*
 * mqreceive.c--
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
#include <stdint.h>

int
main(int argc, char *argv[])
{
    int c, flags;
    mqd_t mqd;
    ssize_t n;
    uint32_t prio;
    void *buf;
    struct mq_attr attr;

    flags = O_RDONLY;
    while ((c = getopt(argc, argv, "n")) != -1) {
        switch (c) {
        case 'n':
            flags |= O_NONBLOCK;
            break;
        }
    }
    if (optind != argc - 1)
        err_quit("usage: mqreceive [ -n ] <name>\n");

    mqd = mq_open(argv[optind], flags);
    if (mqd < 0) {
        err_sys("mq_open error:");
    }
    if (mq_getattr(mqd, &attr) < 0) {
        err_sys("mq_getattr error:");
    }

    buf = malloc(attr.mq_msgsize);
    if (buf == NULL) {
        err_sys("malloc error:");
    }

    n = mq_receive(mqd, buf, attr.mq_msgsize, &prio);
    if (n < 0) {
        err_sys("mq_receive error:");
    }

    printf("read %ld bytes, priority = %u\n", (long)n, prio);

    return 0;
}
