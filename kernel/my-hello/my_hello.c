#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("li yunteng");

static int __init hello_init(void)
{
        printk("hello, init.\n");
        return 0;
}

static void __exit hello_exit(void)
{
        printk("hello, exit.\n");
}

module_init(hello_init);
module_exit(hello_exit);
