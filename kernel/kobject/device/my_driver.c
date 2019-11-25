#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

MODULE_AUTHOR("liyunteng");
MODULE_LICENSE("GPL");

extern struct bus_type my_bus_type;

static int
my_driver_probe(struct device *dev)
{
    printk("driver found device which my driver can handle.\n");
    return 0;
}

static int
my_driver_remove(struct device *dev)
{
    printk("driver found device unpluged.\n");
    return 0;
}

struct device_driver my_driver = {
    .name   = "my_driver",
    .bus    = &my_bus_type,
    .probe  = my_driver_probe,
    .remove = my_driver_remove,
};

static char info_buf[128] = "This is my driver";
static ssize_t
my_driver_show(struct device_driver *driver, char *buf)
{
    return sprintf(buf, "%s", info_buf);
}

static ssize_t
my_driver_store(struct device_driver *driver, const char *buf, size_t count)
{
    return snprintf(info_buf, sizeof(info_buf), "%s", buf);
}

/* static DRIVER_ATTR(drv, S_IRUGO, mydriver_show, NULL); */
static struct driver_attribute my_attr = {
    .attr.name = "info",
    .attr.mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR,
    .show      = my_driver_show,
    .store     = my_driver_store,
};

static int __init
           my_driver_init(void)
{
    int ret = 0;

    driver_register(&my_driver);
    driver_create_file(&my_driver, &my_attr);

    return ret;
}

static void __exit
            my_driver_exit(void)
{
    driver_unregister(&my_driver);
}

module_init(my_driver_init);
module_exit(my_driver_exit);
EXPORT_SYMBOL(my_driver);
