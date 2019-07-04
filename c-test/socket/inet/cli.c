#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int sockfd, n;
    char recvbuf[1024], sendbuf[1024];
    struct sockaddr_in servaddr;

    if (argc != 2) {
        printf("usage: ./client <ipaddress>\n");
        return -1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "create socket failed: %s\n", strerror(errno));
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) <
        0) {
        fprintf(stderr, "connect failed: %s\n", strerror(errno));
        return -1;
    }
    sprintf(sendbuf, "hello %s\n", argv[1]);
    if (send(sockfd, sendbuf, strlen(sendbuf), 0) < 0) {
        fprintf(stderr, "send failed: %s\n", strerror(errno));
        return -1;
    }
    sleep(300);
    close(sockfd);
    return 0;
}
