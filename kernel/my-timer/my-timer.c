#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static int __init my_timer_init(void)
{
	printk("<1>timmer init.\n");
	return 0;
}

static void __exit my_timer_exit(void)
{
	printk("<1>timer exit.\n");
}

module_init(my_timer_init);
module_exit(my_timer_exit);
