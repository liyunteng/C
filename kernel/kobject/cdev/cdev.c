#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

static struct class *cdev_class;
static struct device *cdev_class_dev;

static char info_buf[4096] = {0};
static const char *msg     = "This is a cdev.";

static int
cdev_open(struct inode *inode, struct file *file)
{
    printk("cdev_open\n");
    return 0;
}

static int
cdev_release(struct inode *inode, struct file *file)
{
    printk("cdev_release\n");
    return 0;
}

static ssize_t
cdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    int ret = 0;
    int len = 0;
    printk("cdev_read count: %ld offset: %lld\n", count, *offset);

    len = min(sizeof(info_buf), count);
    if (copy_to_user(buf, info_buf, len)) {
        ret = -EFAULT;
    } else {
        count = len;
        *offset += len;
        ret = len;
    }
    return ret;
}

static ssize_t
cdev_write(struct file *file, const char __user *buf, size_t count,
           loff_t *offset)
{
    int ret = 0;
    int len = 0;
    printk("cdev_write count: %ld offset: %lld\n", count, *offset);

    len = min(sizeof(info_buf), count);
    if (copy_from_user(info_buf, buf, len)) {
        ret = -EFAULT;
    } else {
        count = len;
        *offset += len;
        ret = len;
    }
    return ret;
}

static struct file_operations cdev_fops = {
    .owner   = THIS_MODULE, /* 宏， 指向编译模块时自动创建
                           * 的__this_module变量 */
    .open    = cdev_open,
    .write   = cdev_write,
    .read    = cdev_read,
    .release = cdev_release,
};

int major;

static int
cdev_init(void)
{
    major = register_chrdev(0, "cdev", &cdev_fops);

    cdev_class = class_create(THIS_MODULE, "cdev");

    cdev_class_dev =
        device_create(cdev_class, NULL, MKDEV(major, 0), NULL, "lyt");

    memset(info_buf, 0, sizeof(info_buf));
    snprintf(info_buf, sizeof(info_buf), "%s", msg);
    return 0;
}

static void
cdev_exit(void)
{
    unregister_chrdev(major, "cdev");

    device_unregister(cdev_class_dev);
    class_destroy(cdev_class);
}

module_init(cdev_init);
module_exit(cdev_exit);

MODULE_LICENSE("GPL");
