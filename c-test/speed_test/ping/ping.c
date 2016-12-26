/*
 * ping.c -- ping src
 *
 * Copyright (C) 2016 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2016/04/08 15:27:32
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

#include "ping.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/time.h>


struct itimerval val_alarm = {
        .it_interval.tv_sec = 1,
        .it_interval.tv_usec = 0,
        .it_value.tv_sec = 0,
        .it_value.tv_usec = 1,
};

int main(int argc, char *argv[])
{
        struct hostent *host;

        if (argc  < 2) {
                printf("Usage: %s hostname\n", argv[0]);
                exit(1);
        }

        if ((host = gethostbyname(argv[1])) == NULL) {
                perror("can not understand the host name");
                exit(1);
        }

        hostname = argv[1];

        memset(&dest, 0, sizeof(dest));
        dest.sin_family = PF_INET;
        dest.sin_port = ntohs(0);
        dest.sin_addr = *(struct in_addr *)host->h_addr_list[0];

        if ((sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
                perror("raw socket created error");
                exit(1);
        }

        setuid(getuid());
        pid = getpid();

        set_sighandler();

        printf("Ping %s(%s): %d bytes data in ICMP packets.\n",
               argv[1], inet_ntoa(dest.sin_addr), datalen);

        if ((setitimer(ITIMER_REAL, &val_alarm, NULL)) == -1)
                bail("setitimer fail.");

        recv_reply();

        return 0;

}

void send_ping(void)
{
        struct icmphdr *icmp_hdr;
        int len;

        icmp_hdr = (struct icmphdr *)sendbuf;
        icmp_hdr->type = ICMP_ECHO;
        icmp_hdr->code = 0;
        icmp_hdr->icmp_id = pid;
        icmp_hdr->icmp_seq = nsent++;
        memset(icmp_hdr->data, 0xff, datalen);

        gettimeofday((struct timeval *)icmp_hdr->data, NULL);

        len = ICMP_HSIZE + datalen;
        icmp_hdr->checksum = 0;
        icmp_hdr->checksum = checksum((u8 *)icmp_hdr, len);

        sendto(sockfd, sendbuf, len, 0, (struct sockaddr *)&dest, sizeof(dest));
}

void recv_reply()
{
        int n;
        int errno;
        socklen_t len;

        n = nrecv = 0;
        len = sizeof(from);

        while (nrecv < COUNT) {
                if ((n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                                  (struct sockaddr *)&from, &len)) < 0) {
                        if (errno == EINTR)
                                continue;
                        bail("recvfrom error");
                }

                gettimeofday(&recvtime, NULL);

                if (handle_pkt())
                        continue;
                nrecv++;
        }

        get_statistics(nsent, nrecv);
}

u16 checksum(u8 *buf, int len)
{
        u32 sum = 0;
        u16 *cbuf;

        cbuf = (u16 *)buf;

        while (len > 1) {
                sum += *cbuf++;
                len -= 2;
        }

        if (len)
                sum += *(u8 *)cbuf;

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        return ~sum;
}

int handle_pkt()
{
        struct iphdr *ip;
        struct icmphdr *icmp;


        int ip_hlen;
        u16 ip_datalen;
        double rtt;
        struct timeval *sendtime;

        ip = (struct iphdr *)recvbuf;

        ip_hlen = ip->hlen << 2;
        ip_datalen = ntohs(ip->tot_len) - ip_hlen;

        icmp = (struct icmphdr *)(recvbuf + ip_hlen);
        if (checksum((u8 *)icmp, ip_datalen)) {
                perror("checksum fail");
                return -1;
        }

        if (icmp->icmp_id != pid) {
                perror("error id");
                return -1;
        }

        sendtime = (struct timeval *)icmp->data;
        rtt = ((&recvtime)->tv_sec - sendtime->tv_sec) * 1000 +
                ((&recvtime)->tv_usec - sendtime->tv_usec)/1000.0;
        printf("%d bytes from %s: icmp_seq = %u ttl=%d rtt=%.3f ms\n",
               ip_datalen,
               inet_ntoa(from.sin_addr),
               icmp->icmp_seq,
               ip->ttl,
               rtt);

        return 0;
}

void set_sighandler()
{
        act_alarm.sa_handler = alarm_handler;
        if (sigaction(SIGALRM, &act_alarm, NULL) == -1)
                bail("SIGALRM handler setting fail.");

        act_int.sa_handler = int_handler;
        if (sigaction(SIGINT, &act_int, NULL) == -1)
                bail("SIGINT handler setting fail.");
}


void get_statistics(int nsent, int nrecv)
{
        printf("---%s ping statistics ---\n", inet_ntoa(dest.sin_addr));
        printf("%d packets transmitted, %d received, %0.0f%%""packet loss\n",
               nsent, nrecv, 1.0 * (nsent - nrecv)/nsent * 100);
}

void bail(const char *on_what)
{
        fputs(strerror(errno), stderr);
        fputs(":", stderr);
        fputs(on_what, stderr);
        fputc('\n', stderr);
        exit(1);
}


void int_handler(int sig)
{
        get_statistics(nsent, nrecv);
        close(sockfd);
        exit(1);
}

void alarm_handler(int signo)
{
        send_ping();
}
