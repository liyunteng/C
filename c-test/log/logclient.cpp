/*
 * logclient.cpp -- recv log
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/12/13 06:06:13
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

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define LOGPORT 12345

int main(int argc, char *argv[])
{
        int sockfd;
        struct sockaddr_in addr;
        struct sockaddr_in srcaddr;
        char buf[1024];

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
                perror("socket");
                return sockfd;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(LOGPORT);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
                perror("bind");
                return -1;
        }

        socklen_t addrlen;
        while(recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&srcaddr, &addrlen)) {
                fprintf(stderr, "from: %s recv: %s",inet_ntoa(srcaddr.sin_addr), buf);
        }

        return 0;
}
