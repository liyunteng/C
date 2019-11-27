#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/sysfs.h>

MODULE_AUTHOR("liyunteng");
MODULE_LICENSE("GPL");

struct kset *kset_p;
struct kset kset_c;

void obj_test_release(struct kobject *);
ssize_t kobj_test_show(struct kobject *, struct attribute *, char *);
ssize_t kobj_test_store(struct kobject *, struct attribute *, const char *,
                        size_t);

static char attr_buf[128] = "abc";

static struct attribute test_attr = {
    .name = "kobj_test_config",
    .mode = S_IRWXUGO,
};

static struct attribute *def_attrs[] = {
    &test_attr,
    NULL,
};

static struct sysfs_ops obj_test_sysops = {
    .show  = kobj_test_show,
    .store = kobj_test_store,
};

static struct kobj_type ktype = {
    .release       = obj_test_release,
    .sysfs_ops     = &obj_test_sysops,
    .default_attrs = def_attrs,
};

void
obj_test_release(struct kobject *kobject)
{
    printk("kobj_test: release.\n");
}

ssize_t
kobj_test_show(struct kobject *kobject, struct attribute *attr, char *buf)
{
    printk("show attrname: %s.\n", attr->name);
    sprintf(buf, "%s\n", attr_buf);
    return strlen(buf) + 1;
}

ssize_t
kobj_test_store(struct kobject *kobject, struct attribute *attr,
                const char *buf, size_t size)
{
    printk("store attrname: %s.\n", attr->name);
    printk("write: %s.\n", buf);
    snprintf(attr_buf, sizeof(attr_buf), "%s", buf);
    return size;
}

static int
kset_filter(struct kset *kset, struct kobject *kobj)
{
    printk("Filter: %s.\n", kobj->name);
    return 1;
}

static const char *
kset_name(struct kset *kset, struct kobject *kobj)
{
    static char buf[20];
    printk("Name: %s.\n", kobj->name);
    sprintf(buf, "%s", kobj->name);
    return buf;
}

static int
kset_uevent(struct kset *kset, struct kobject *kobj,
            struct kobj_uevent_env *env)
{
    int i = 0;
    printk("Uevent: %s.\n", kobj->name);

    while (i < env->envp_idx) {
        printk("%s.\n", env->envp[i]);
        i++;
    }

    return 0;
}

static struct kset_uevent_ops uevent_ops = {
    .filter = kset_filter,
    .name   = kset_name,
    .uevent = kset_uevent,
};

static int __init
kset_test_init(void)
{
    int ret = 0;

    printk("kset test init.\n");

    kset_p = kset_create_and_add("kset_p", &uevent_ops, NULL);

    kobject_set_name(&kset_c.kobj, "kset_c");
    kset_c.kobj.kset = kset_p;

    kset_c.kobj.ktype = &ktype;
    ret               = kset_register(&kset_c);

    if (ret)
        kset_unregister(kset_p);

    return 0;
}
static void __exit
kset_test_exit(void)
{
    printk("kset test exit.\n");
    kset_unregister(kset_p);
    kset_unregister(&kset_c);
}
module_init(kset_test_init);
module_exit(kset_test_exit);
