/*
 * send.c - send
 *
 * Date   : 2019/11/13
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/tcp.h>


int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IPaddress> <Port>\n", argv[0]);
        return -1;
    }

    struct sockaddr_in servaddr;
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    int keep_alive = 1;
    int keep_idel = 5, keep_interval = 1, keep_count = 3;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)) < 0) {
        perror("keep alive");
        return -1;
    }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idel, sizeof(keep_idel)) < 0) {
        perror("keep idel");
        return -1;
    }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval)) < 0) {
        perror("keep interval");
        return -1;
    }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count)) < 0) {
        perror("keep count");
        return -1;
    }
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_USER_TIMEOUT, &tv, sizeof(tv)) < 0) {
        perror("setsockopt");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons((short)atoi(argv[2]));

    if (connect(sockfd, (struct sockaddr*)&servaddr, (socklen_t)sizeof(servaddr)) < 0 ) {
        perror("connect");
        return -1;
    }
    while (1) {
#if 1
        char buf[] = "hello\n";
        int n = send(sockfd, buf, strlen(buf), 0);
        if (n <= 0) {
            perror("send");
            return errno;
        }
        printf("send %d\n", n);
#endif
        sleep(2);
    }
    return 0;
}
