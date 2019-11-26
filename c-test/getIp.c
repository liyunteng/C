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

typedef struct {
    char ip[14];
    char netmask[14];
    char ifname[16];
    unsigned char mac[6];
    int16_t flags;
} ip_info_t;

int getIp(const char *ifname, ip_info_t *info)
{
    int sockfd;
    struct ifreq ifr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }
    strcpy(info->ifname, ifname);
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        return -1;
    }
    printf("DEV: %s ", ifname);
    info->flags = ifr.ifr_flags;
    if (ifr.ifr_flags & IFF_UP) {
        printf("UP ");
    }
    if (ifr.ifr_flags & IFF_BROADCAST) {
        printf("BROADCAST ");
    }
    if (ifr.ifr_flags & IFF_DEBUG) {
        printf("DEBUG ");
    }
    if (ifr.ifr_flags & IFF_LOOPBACK) {
        printf("LOOPBACK ");
    }
    if (ifr.ifr_flags & IFF_POINTOPOINT) {
        printf("POINTOPOINT ");
    }
    if (ifr.ifr_flags & IFF_RUNNING) {
        printf("RUNNING ");
    }
    if (ifr.ifr_flags & IFF_MULTICAST) {
        printf("MULTICAST ");
    }
    printf("\n");

    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0) {
        return -1;
    }
    memcpy(info->mac, ifr.ifr_hwaddr.sa_data, sizeof(info->mac));
    printf("MAC: %02X-%02X-%02X-%02X-%02X-%02X\n",
           info->mac[0], info->mac[1], info->mac[2],
           info->mac[3], info->mac[4], info->mac[5]);

    if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0) {
        return -1;
    }
    strcpy(info->ip, inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr));
    printf("IP: %s\n", info->ip);
    if (ioctl(sockfd, SIOCGIFNETMASK, &ifr) < 0) {
        return -1;
    }
    strcpy(info->netmask, inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr));
    printf("NETMASK: %s\n", info->netmask);
    return 0;
}

int
main(int argc, char *argv[])
{
    ip_info_t info;
    const char *ifname = "eth0";
    if (argc == 2) {
        ifname = argv[1];
    }
    getIp(ifname, &info);
}
