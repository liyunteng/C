#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/sysfs.h>

MODULE_AUTHOR("liyunteng");
MODULE_LICENSE("GPL");

void obj_test_release(struct kobject *kobject);
ssize_t kobj_test_show(struct kobject *kobject, struct attribute *attr,
                       char *buf);
ssize_t kobj_test_store(struct kobject *kobject, struct attribute *attr,
                        const char *buf, size_t count);

static char attr_buf[] = "abc";

struct attribute test_attr = {
    .name = "kobj_test_config",
    .mode = S_IRWXUGO,
};

static struct attribute *def_attrs[] = {
    &test_attr,
    NULL,
};

struct sysfs_ops obj_test_sysops = {
    .show  = kobj_test_show,
    .store = kobj_test_store,
};

struct kobj_type ktype = {
    .release       = obj_test_release,
    .sysfs_ops     = &obj_test_sysops,
    .default_attrs = def_attrs,
};

void
obj_test_release(struct kobject *kobject)
{
    printk("kobject_test: release.\n");
}

ssize_t
kobj_test_show(struct kobject *kobject, struct attribute *attr, char *buf)
{
    printk("show attrname: %s\n", attr->name);
    sprintf(buf, "%s\n", attr_buf);
    return strlen(buf) + 1;
}

ssize_t
kobj_test_store(struct kobject *kobject, struct attribute *attr,
                const char *buf, size_t count)
{
    printk("store attrname: %s\n", attr->name);
    printk("write: %s\n", buf);
    snprintf(attr_buf, sizeof(attr_buf), "%s", buf);
    return count;
}

struct kobject kobj;

static int __init
kobj_test_init(void)
{
    printk("kobject test init.\n");
    kobject_init_and_add(&kobj, &ktype, NULL, "kobject_test");
    return 0;
}

static void __exit
kobj_test_exit(void)
{
    printk("kobject test exit.\n");
    kobject_del(&kobj);
}

module_init(kobj_test_init);
module_exit(kobj_test_exit);
