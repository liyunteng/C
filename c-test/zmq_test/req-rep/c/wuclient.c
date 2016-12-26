/*
 * wuclient.c -- weather update
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/26 09:44:15
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

#include <zmq.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
        void *context = zmq_ctx_new();

        printf("Collecting updates from weather server...\n");
        void *subscriber = zmq_socket(context, ZMQ_SUB);
        int rc = zmq_connect(subscriber, "tcp://localhost:5556");
        assert(rc == 0);


        char *filter = (argc > 1) ? argv[1] : "10001 ";
        rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
        assert(rc == 0);


        int update_nbr;
        long total_temp = 0;
        char buf[36];
        for (update_nbr = 0; update_nbr < 100; update_nbr++) {
                memset(buf, 0, sizeof(buf));
                zmq_recv(subscriber, buf, sizeof(buf), 0);
                printf("receive %s\n", buf);

                int zipcode, temp, rel;
                sscanf(buf, "%d %d %d", &zipcode, &temp, &rel);
                total_temp += temp;
        }


        printf("Average temp for zipcode %s is %dF\n",
               filter, (int)(total_temp/update_nbr));

        zmq_close(subscriber);
        zmq_ctx_destroy(context);
        return 0;
}
