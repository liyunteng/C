/*******************************************************************************
* Author : liyunteng
* Email : li_yunteng@163.com
* Created Time : 2014-01-17 09:55
* Filename : nl_ker.c
* Description : 
* *****************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <net/netlink.h>

#define NETLINK_TEST 23


struct sock *nl_sk = NULL;
struct netlink_kernel_cfg nlcfg;

void kernel_fuc(struct sk_buff *skb)
{
    struct nlmsghdr *nlhdr;
    int pid;

    nlhdr = nlmsg_hdr(skb);
    if (nlhdr->nlmsg_type == NETLINK_TEST) {
	pid = nlhdr->nlmsg_pid;
	printk("%s:received process %d's message:%s\n",
	       __FUNCTION__, pid, (char *) NLMSG_DATA(nlhdr));
	kfree_skb(skb);

	skb = alloc_skb(NLMSG_SPACE(20), GFP_KERNEL);
	if (skb == NULL) {
	    printk(KERN_ERR "nl_ker: alloc_skb failed.\n");
	    return;
	}

	nlhdr = nlmsg_hdr(skb);
	nlhdr->nlmsg_pid = 0;
	memcpy((char *) NLMSG_DATA(nlhdr), "yes, we do it!", 20);
	netlink_unicast(nl_sk, skb, pid, MSG_DONTWAIT);
    }
}

static int __init nl_test_init(void)
{
    nlcfg.groups = 0;
    nlcfg.flags = 0;
    nlcfg.input = kernel_fuc;

    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &nlcfg);
    if (!nl_sk) {
	printk(KERN_ERR "netlink_kernel_create error.\n");
	return -EIO;
    }
    printk("nl_ker init.\n");
    return 0;
}

static void __exit nl_test_exit(void)
{
    if (nl_sk != NULL) {
	netlink_kernel_release(nl_sk);
    }
    printk("nl_ker exit.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lyt");
module_init(nl_test_init);
module_exit(nl_test_exit);
