#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <linux/netlink.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define UEVENT_BUFFER_SIZE 2048

static int
init_hotplug_sock()
{
    const int buffersize = 1024;
    int       ret;

    struct sockaddr_nl snl;
    bzero(&snl, sizeof(struct sockaddr_nl));
    snl.nl_family = PF_NETLINK;
    snl.nl_pid    = getpid();
    snl.nl_groups = 1;

    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));

    ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
    if (ret < 0) {
        perror("bind");
        close(s);
        return -1;
    }
    return s;
}

int
main(int argc, char *argv[])
{
    int hotplug_sock = init_hotplug_sock();

    while (1) {
        char buf[UEVENT_BUFFER_SIZE * 2] = {0};
        recv(hotplug_sock, &buf, sizeof(buf), 0);
        printf("%s\n", buf);
    }
    return 0;
}
