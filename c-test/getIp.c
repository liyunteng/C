/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2015-08-18 10:50
 * Filename : test.c
 * Description :
 * *****************************************************************************/
#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <net/route.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#define UPDATE_SERVER "upgrade.streamocean.com"
#define UPDATE_PORT 80
#define UPDATE_DIR "/yum/ihi/client"

int
main(void)
{
    int           sockfd;
    struct ifreq  ifr;
    unsigned char mac[6];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "create sock failed: %s\n", strerror(errno));
        return errno;
    }

    strcpy(ifr.ifr_name, "eth0");
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        fprintf(stderr, "ioctl failed\n");
        return -1;
    }
    printf("dev: %s ", ifr.ifr_name);
    if (ifr.ifr_flags & IFF_UP) {
        printf("up\n");
    } else {
        printf("down\n");
    }

    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0) {
        fprintf(stderr, "ioctl failed\n");
        return -1;
    }
    memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
    printf("MAC: %02X-%02X-%02X-%02X-%02X-%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0) {
        fprintf(stderr, "ioctl failed\n");
        return -1;
    }
    printf("ip: %s\n", inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr));

    if (ioctl(sockfd, SIOCGIFNETMASK, &ifr) < 0) {
        fprintf(stderr, "ioctl failed\n");
        return -1;
    }
    printf("netmask: %s\n", inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr));

    return 0;
}
