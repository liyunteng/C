#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#define MEMDEV_MAJOR 110
#define MEMDEV_NR_DEVS 4
#define MEMDEV_SIZE 4096

struct mem_dev {
    struct cdev cdev;
    int         size;
    void *      data;
};

static int            mem_major = MEMDEV_MAJOR;
struct mem_dev *      mem_devp;
static struct class * memdev_class;
static struct device *memdev_device;

/* 文件打开函数 */
int
mem_open(struct inode *inode, struct file *filp)
{
    struct mem_dev *dev;

    /* 获取次设备号 */
    int num = MINOR(inode->i_rdev); /* 只有在open中能拿到inode提取
                                     * num */
    if (num >= MEMDEV_NR_DEVS)
        return -ENODEV;

    dev = &mem_devp[num]; /* 将对应dev保存到filp->privata_data
                           * */
    filp->private_data = dev;
    return 0;
}

/* 文件释放函数 */
int
mem_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/* 读函数 */
static ssize_t
mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p     = *ppos;
    unsigned int  count = size;
    int           ret   = 0;

    struct mem_dev *dev = filp->private_data; /* 获取设备结构提指
                                               * 针
                                               */
    /* 判断位置是否有效 */
    if (p >= MEMDEV_SIZE) /* 读的位置大于设备大小 -- 无效 */
        return 0;
    if (count > MEMDEV_SIZE - p) /* 要读的大小 > 设备剩下的大小 */
        count = MEMDEV_SIZE - p;
    /* 读数据到用户空间 */
    if (copy_to_user(buf, (void *)(dev->data + p), count)) {
        ret = -EFAULT;
    } else {
        *ppos += count; /* 读后，改写读指针位置 */
        ret = count;

        printk(KERN_INFO "read %d bytes form %ld\n%s\n", count, p, (char *)dev->data + p);
    }
    return ret;
}

/* 写函数 */
static ssize_t
mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p     = *ppos;
    unsigned int  count = size;
    int           ret   = 0;

    struct mem_dev *dev = filp->private_data;

    /* 分析和获取有效的写长度 */
    if (p >= MEMDEV_SIZE)
        return 0;
    if (count > MEMDEV_SIZE - p)
        count = MEMDEV_SIZE - p;
    /* 从用户空间写入数据 */
    if (copy_from_user(dev->data + p, buf, count)) {
        ret = -EFAULT;
    } else {
        *ppos += count;
        ret = count;

        printk(KERN_INFO "written %d bytes from %ld\n%s\n", count, p, (char *)dev->data + p);
    }

    return ret;
}

/* seek文件定位函数 */
static loff_t
mem_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t newpos;

    switch (whence) {
    case 0: /* SEEK_SET */
        newpos = offset;
        break;
    case 1: /* SEEK_CUR */
        newpos = filp->f_pos + offset;
        break;
    case 2: /* SEEK_END */
        newpos = MEMDEV_SIZE - 1 + offset;
        break;
    default:
        return -EINVAL;
    }

    if ((newpos < 0) || (newpos > MEMDEV_SIZE))
        return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}

/* 文件操作结构体 */
static const struct file_operations mem_fops = {
    .owner   = THIS_MODULE,
    .llseek  = mem_llseek,
    .read    = mem_read,
    .write   = mem_write,
    .open    = mem_open,
    .release = mem_release,
};

/* 设备驱动模块加载函数 */
static int
memdev_init(void)
{
    int  result;
    int  i;
    char buf[20];
    /* 利用主设备号，次设备号构造设备号 */
    dev_t devno = MKDEV(mem_major, 0);

    /* 静态申请设备号 */
    if (mem_major)
        result = register_chrdev_region(devno, MEMDEV_NR_DEVS, "memdev");
    else { /* 动态分配设备号 */
        printk("this way\n");
        result    = alloc_chrdev_region(&devno, 0, MEMDEV_NR_DEVS, "memdev");
        mem_major = MAJOR(devno); /* 提取主设备号 */
    }

    if (result < 0)
        return result;

    /* 注册字符设备 */
    mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);
    if (!mem_devp) {
        result = -ENOMEM;
        goto fail_malloc;
    }

    memset(mem_devp, 0, sizeof(struct mem_dev));
    memdev_class = class_create(THIS_MODULE, "memdev");
    /* 为设备分配内存 */
    for (i = 0; i < MEMDEV_NR_DEVS; i++) {
        cdev_init(&mem_devp[i].cdev, &mem_fops);
        mem_devp[i].cdev.owner = THIS_MODULE;
        mem_devp[i].cdev.ops   = &mem_fops;
        cdev_add(&mem_devp[i].cdev, MKDEV(mem_major, i), 1);
        mem_devp[i].size = MEMDEV_SIZE;
        mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
        memset(mem_devp[i].data, 0, MEMDEV_SIZE);

        sprintf(buf, "memdev%d", i);
        memdev_device = device_create(memdev_class, NULL, MKDEV(mem_major, i), NULL, buf);
    }
    printk("init done\n");
    return 0;
fail_malloc:
    unregister_chrdev_region(devno, MEMDEV_NR_DEVS);
    return result;
}

/* 模块卸载 */
static void
memdev_exit(void)
{
    int i;
    for (i = 0; i < MEMDEV_NR_DEVS; i++) {
        cdev_del(&mem_devp[i].cdev);
    }
    kfree(mem_devp);
    unregister_chrdev_region(MKDEV(mem_major, 0), MEMDEV_NR_DEVS);
    for (i = 0; i < MEMDEV_NR_DEVS; i++) {
        device_destroy(memdev_class, MKDEV(mem_major, i));
    }
    class_destroy(memdev_class);
}

MODULE_AUTHOR("liyunteng");
MODULE_LICENSE("GPL");
module_init(memdev_init);
module_exit(memdev_exit);

/* Local Variables: */
/* compile-command: "make -k " */
/* End: */
