#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li yunteng");

extern unsigned long volatile jiffies;

static int __init jiffies_init(void)
{
        printk("jiffies_init %lu.\n" ,jiffies);
        return 0;
}

static void __exit jiffies_exit(void)
{
        printk("jiffies_exit %lu.\n", jiffies);
}

module_init(jiffies_init);
module_exit(jiffies_exit);
