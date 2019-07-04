/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-01-17 10:19
 * Filename : nl_usr.c
 * Description :
 * *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>

#define NETLINK_TEST 23

struct netlink_info {
    struct nlmsghdr nlhdr;
    char buf[20];
};

int main(void)
{

    int fd, len1, len2;
    struct sockaddr_nl src_addr, dst_addr;
    struct nlmsghdr *nlhdr;
    struct msghdr hdr;
    struct netlink_info *info;

    fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if (fd < 0) {
        printf("create netlink socket failed!\n");
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = PF_NETLINK;
    src_addr.nl_pid = getpid();
    src_addr.nl_groups = 0;

    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.nl_family = PF_NETLINK;
    dst_addr.nl_pid = 0;
    src_addr.nl_groups = 0;

    if (bind(fd, (struct sockaddr *) &src_addr, sizeof(src_addr)) != 0) {
        perror("bind error!\n");
        return -1;
    }

    nlhdr = (struct nlmsghdr *) malloc(NLMSG_SPACE(20));
    nlhdr->nlmsg_len = NLMSG_LENGTH(20);
    nlhdr->nlmsg_flags = 0;
    nlhdr->nlmsg_type = NETLINK_TEST;
    nlhdr->nlmsg_pid = src_addr.nl_pid;
    strcpy((char *) NLMSG_DATA(nlhdr), "hello, you!");

    memset(&hdr, 0, sizeof(hdr));
    sendto(fd, nlhdr, nlhdr->nlmsg_len, 0, (struct sockaddr *) &dst_addr,
           sizeof(dst_addr));

    free(nlhdr);

    info = (struct netlink_info *) malloc(sizeof(struct netlink_info));
    if (info == NULL) {
        perror("info malloc failed!\n");
        return -1;
    }

    while (1) {
        len1 = sizeof(struct sockaddr_nl);
        len2 = recvfrom(fd, info, sizeof(struct netlink_info), 0,
                        (struct sockaddr *) &dst_addr,
                        (socklen_t *) & len1);
        if (len2 > 0) {
            printf("process %d received %s\n", info->nlhdr.nlmsg_pid,
                   info->buf);
            return 0;
        }
    }
}
