#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define port 6666

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    struct sockaddr_in servaddr1, servaddr2;
    char buf[1024];
    int n;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	fprintf(stderr, "create socket failed: %s\n", strerror(errno));
	return -1;
    }

    memset(&servaddr1, 0, sizeof(servaddr1));
    servaddr1.sin_family = AF_INET;
    servaddr1.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &servaddr1.sin_addr);

    memset(&servaddr2, 0, sizeof(servaddr2));
    servaddr2.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr2.sin_addr);
    servaddr2.sin_port = htons(port);

    /* if (bind(listenfd,(struct sockaddr *)&servaddr1, sizeof(servaddr1)) == -1) { */
    /*      fprintf(stderr, "bind  servaddr1 failed: %s\n", */
    /*              strerror(errno)); */
    /*      return -1; */
    /* } */
    if (bind(listenfd, (struct sockaddr *) &servaddr2, sizeof(servaddr2))
	== -1) {
	fprintf(stderr, "bind servaddr2 failed: %s\n", strerror(errno));
	return -1;
    }

    if (listen(listenfd, 10) == -1) {
	fprintf(stderr, "listen failed: %s\n", strerror(errno));
	return -1;
    }

    while (1) {
	if ((connfd =
	     accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1) {
	    fprintf(stderr, "accept failed : %s\n", strerror(errno));
	    continue;
	}

	n = recv(connfd, buf, 1024, 0);
	buf[n] = '\0';
	printf("data:: %s\n", buf);
	close(connfd);
    }
    close(listenfd);
    return 0;
}
