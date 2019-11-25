/*
 * mqsend.c--
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
    mqd_t    mqd;
    void *   ptr;
    size_t   len;
    uint32_t prio;

    if (argc != 4) {
        err_quit("usage: mqsend <name> <#bytes> <priority>\n");
    }

    len  = atoi(argv[2]);
    prio = atoi(argv[3]);
    mqd  = mq_open(argv[1], O_WRONLY);
    if (mqd < 0) {
        err_sys("mq_open error:");
    }
    ptr = calloc(len, sizeof(char));
    if (ptr == NULL)
        err_sys("calloc error:");

    if (mq_send(mqd, ptr, len, prio) < 0) {
        err_sys("mq_send error:");
    }

    return 0;
}
