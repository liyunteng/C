#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

MODULE_AUTHOR("liyunteng");
MODULE_LICENSE("GPL");

static char *Version = "$Revision: 1.0$";

static int
my_match(struct device *dev, struct device_driver *driver)
{
    return !strncmp(dev->kobj.name, driver->name, strlen(driver->name));
}

struct bus_type my_bus_type = {
    .name  = "my_bus",
    .match = my_match,
};

static ssize_t
show_bus_version(struct bus_type *bus, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%s\n", Version);
}

static BUS_ATTR(version, S_IRUGO, show_bus_version, NULL);

static int __init
my_bus_init(void)
{
    int ret;

    ret = bus_register(&my_bus_type);
    if (ret)
        return ret;

    if (bus_create_file(&my_bus_type, &bus_attr_version))
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

/* Local Variables: */
/* compile-command: "make -k " */
/* End: */
