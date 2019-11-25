#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

MODULE_AUTHOR("liyunteng");
MODULE_LICENSE("GPL");

static char *Version = "$Revision: 1.0$";

static int
my_bus_match(struct device *dev, struct device_driver *driver)
{
    return strncmp(dev->kobj.name, driver->name, strlen(driver->name));
}

static int
my_bus_probe(struct device *dev)
{
    printk("my_bus probe\n");
    return 0;
}

static int
my_bus_remove(struct device *dev)
{
    printk("my_bus remove\n");
    return 0;
}

static int
my_bus_online(struct device *dev)
{
    printk("my_bus online\n");
    return 0;
}

static int
my_bus_offline(struct device *dev)
{
    printk("my_bus offline\n");
    return 0;
}

struct bus_type my_bus_type;
static int
my_bus_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    int i = 0;
    printk("my_bus uevent\n");
    while (i < env->envp_idx) {
        printk("%s.\n", env->envp[i]);
        i++;
    }

    return 0;
}

struct bus_type my_bus_type = {
    .name  = "my_bus",
    .match = my_bus_match,
    .uevent = my_bus_uevent,
    .probe = my_bus_probe,
    .remove = my_bus_remove,
    .online = my_bus_online,
    .offline = my_bus_offline
};

static ssize_t
my_bus_attr_show_version(struct bus_type *bus, char *buf)
{
    ssize_t size = sprintf(buf, "%s", Version);
    return size;
}

static struct bus_attribute my_attr = {
    .attr.name = "version",
    .attr.mode = S_IRUSR | S_IRGRP | S_IROTH,
    .show = my_bus_attr_show_version,
};

static int __init
my_bus_init(void)
{
    int ret;

    ret = bus_register(&my_bus_type);
    if (ret)
        return ret;
    ret = bus_create_file(&my_bus_type, &my_attr);
    if (ret)
        printk(KERN_NOTICE "fail to create version attribute.\n");

    return ret;
}

static void __exit
my_bus_exit(void)
{
    bus_unregister(&my_bus_type);
}

module_init(my_bus_init);
module_exit(my_bus_exit);

EXPORT_SYMBOL(my_bus_type);
