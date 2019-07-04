/*
 * test.c -- test http request
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/06/29 18:46:26
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
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 80
#define BUFSIZE 1024

int main(void)
{
    int sockfd, ret, i, h;
    struct sockaddr_in servaddr;
    char str1[4096], buf[BUFSIZE];
    fd_set t_set1;
    struct timeval tv;
    struct hostent *host;
    struct in_addr addr;
    char *ip = NULL;

    if ((host = gethostbyname("www.streamocean.com"))) {
        char *p = host->h_addr_list[0];
        memcpy(&addr.s_addr, p, host->h_length);
        ip = inet_ntoa(addr);
        printf("ip: %s\n", ip);
    } else {
        printf("error.\n");
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf ("创建网络连接失败,本线程即将终止---socket error!\n");
        exit(0);
    };

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
        printf ("创建网络连接失败,本线程即将终止--inet_pton error!\n");
        exit(0);
    };

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("连接到服务器失败,connect error!\n");
        exit(0);
    }
    printf("与远端建立了连接\n");

    //发送数据
    memset(str1, 0, 4096);
    strcat(str1, "GET /SOTP/index.php/Interface/estimate/estimate/dev/dtest20150326-3/user/bjdx_2 HTTP/1.1\r\n");
    /* strcat(str1, "GET /SOTP/index.php/Interface/content/content/dev/dtest20150326-3/user/bjdx_2/vsp/http%3A%2F%2F172.16.3.121%3A8080%2F/ HTTP/1.1\r\n"); */
    strcat(str1, "Accept: */*\r\n");
    strcat(str1, "User-Agent: Mozilla/5.0\r\n");
    strcat(str1, "Host: ");
    strcat(str1, ip);
    strcat(str1, "\r\n");
    strcat(str1, "Connection: Keep-Alive\r\n\r\n");

    printf("%s", str1);

    ret = write(sockfd, str1, strlen(str1));
    if (ret < 0) {
        printf("发送失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
        exit(0);
    } else {
        printf("消息发送成功，共发送了%d个字节！\n\n", ret);
    }

    /*
     * FD_ZERO(&t_set1);
     * FD_SET(sockfd, &t_set1);
     *
     * while(1){
     *     sleep(2);
     *     tv.tv_sec= 0;
     *     tv.tv_usec= 0;
     *     h= 0;
     *     printf("--------------->1");
     *     h= select(sockfd +1, &t_set1, NULL, NULL, &tv);
     *     printf("--------------->2");
     *
     *     //if (h == 0) continue;
     *     if (h < 0) {
     *         close(sockfd);
     *         printf("在读取数据报文时SELECT检测到异常，该异常导致线程终止！\n");
     *         return -1;
     *     };
     *
     *     if (h > 0){
     *         memset(buf, 0, 4096);
     *         i= read(sockfd, buf, 4095);
     *         if (i==0){
     *             close(sockfd);
     *             printf("读取数据报文时发现远端关闭，该线程终止！\n");
     *             return -1;
     *         }
     *
     *         printf("%s\n", buf);
     *     }
     * }
     */

    while (read(sockfd, buf, 4095) != 0) {
        printf("%s\n", buf);
    }

    close(sockfd);


    return 0;
}
