#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

MODULE_AUTHOR("liyunteng");
MODULE_LICENSE("GPL");

extern struct bus_type my_bus_type;
extern struct device_driver my_driver;

static void
my_dev_release(struct device *dev)
{
    printk("my_dev release\n");
}

struct device my_dev = {
    .init_name = "my_dev",
    .bus       = &my_bus_type,
    .driver    = &my_driver,
    .release   = my_dev_release,
};

static char info_buf[] = "This is my device.";
static ssize_t
my_dev_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%s", info_buf);
}

static ssize_t
my_dev_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return snprintf(info_buf, sizeof(info_buf), "%s", buf);
}

static struct device_attribute my_att = {
    .attr.name = "info",
    .attr.mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR,
    .show = my_dev_show,
    .store = my_dev_store,
};

static int __init
my_device_init(void)
{
    int ret = 0;

    device_register(&my_dev);
    device_create_file(&my_dev, &my_att);
    return ret;
}

static void
my_device_exit(void)
{
    device_unregister(&my_dev);
}

module_init(my_device_init);
module_exit(my_device_exit);
