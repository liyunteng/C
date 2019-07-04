/*
 * client.c --
 *
 * Copyright (C) 2016 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2016/04/08 17:31:19
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
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <inttypes.h>
#include <netdb.h>
#include <signal.h>

#include "comm.h"
#include "speed.h"

static int flag = 0;

void speed_alarm_handler(int sig)
{
    flag++;
}

int tcp_speed_test(const char *h, const int port, int interval)
{
    char sendbuf[BUFFSIZE];
    char recvbuf[BUFFSIZE];
    unsigned long long nsent;
    unsigned long long nrecv;
    pid_t pid;
    int sockfd;
    struct sockaddr_in dest;
    struct sockaddr_in from;

    struct hostent *host;
    if ((host = gethostbyname(h)) == NULL) {
        perror("gethostbyname failed");
        return -1;
    }

    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr = *(struct in_addr *) host->h_addr_list[0];

    struct sigaction speed_act_pipe;
    speed_act_pipe.sa_flags = 0;
    speed_act_pipe.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &speed_act_pipe, NULL) == -1) {
        perror("sigaction");
        return -1;
    }


    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket create");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) < 0) {
        perror("connect");
        return -1;
    }

    struct sigaction speed_act_alarm;
    speed_act_alarm.sa_flags = 0;
    speed_act_alarm.sa_handler = speed_alarm_handler;
    if (sigaction(SIGALRM, &speed_act_alarm, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    struct itimerval speed_val_alarm;
    speed_val_alarm.it_interval.tv_sec = interval;
    speed_val_alarm.it_interval.tv_usec = 0;
    speed_val_alarm.it_value.tv_sec = interval;
    speed_val_alarm.it_value.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &speed_val_alarm, NULL) == -1) {
        perror("setitimer:");
        return -1;
    }

    uint64_t count = 0;
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    while (flag == 0) {
        memset(sendbuf, 0xff, sizeof(sendbuf));
        struct myhdr *hdr = (struct myhdr *) sendbuf;
        hdr->type = MSG_SEND;
        hdr->seq = nsent;
        hdr->id = getpid();
        gettimeofday((struct timeval *) hdr->data, NULL);

        int n =
            sendto(sockfd, sendbuf, sizeof(sendbuf), 0,
                   (struct sockaddr *) &from, sizeof(from));
        if (n != sizeof(sendbuf)) {
            if (errno == EINTR) {
                nsent++;
                count += n;
                continue;
            }
            if (errno == EPIPE || errno == ECONNRESET) {
                fprintf(stderr, "server closed\n");
                close(sockfd);
                return errno;
            }
            if (errno == 0)
                continue;
            fprintf(stderr, "send %llu failed: %s.\n", nsent,
                    strerror(errno));
            continue;
        }
        nsent++;
        count += sizeof(sendbuf);
    }

    shutdown(sockfd, SHUT_WR);
    gettimeofday(&tv2, NULL);
    unsigned long long t =
        (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec -
                                            tv1.tv_usec) / 1000;
    if (t == 0)
        t = 1;
    fprintf(stderr,
            "Upload size: %" PRIu64 " nsent: %llu time: %llu ms speed: %"
            PRIu64 " kB/s\n", count, nsent, t,
            (uint64_t) (count / t * 1000 / 1024));

    //sendto(sockfd, sendbuf, 1, 0, (struct sockaddr *)&dest, sizeof(dest));

    count = 0;
    nrecv = 0;
    gettimeofday(&tv1, NULL);
    socklen_t len = sizeof(from);
    while (flag == 1) {
        int n =
            recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                     (struct sockaddr *) &from, &len);
        if (n > 0) {
            count += n;
            nrecv++;
        } else if (n == 0) {
            fprintf(stderr, "server closed.\n");
            break;
        } else if (n < 0) {
            if (errno == EINTR) {
                n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                             (struct sockaddr *) &from, &len);
                if (n > 0) {
                    count += n;
                    nrecv++;
                }

            } else {
                fprintf(stderr, "recv failed: %s\n", strerror(errno));
            }
        }
    }
    shutdown(sockfd, SHUT_RD);

    gettimeofday(&tv2, NULL);
    if (t == 0)
        t = 1;
    t = (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec -
                                            tv1.tv_usec) / 1000;
    fprintf(stderr,
            "Download size: %" PRIu64
            " nrecv: %llu time: %llums ms speed: %" PRIu64 " kB/s\n",
            count, nrecv, t, (uint64_t) (count / t * 1000 / 1024));

    close(sockfd);
    return 0;
}
