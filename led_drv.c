#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
// 定义设备号
#define NAME "chrdev_led"

int major = 0; // 保存主设备号

// 函数实现
int myopen(struct inode *node, struct file *file_t)
{
    printk("%s %s %d\n", __FILE__, __func__, __LINE__);
    return 0;
}
ssize_t myread(struct file *file_t, char __user *ubuf, size_t n, loff_t *off_t)
{
    printk("%s %s %d\n", __FILE__, __func__, __LINE__);
    return 0;
}
ssize_t mywrite(struct file *file_t, const char __user *ubuf, size_t n, loff_t *off_t)
{
    printk("%s %s %d\n", __FILE__, __func__, __LINE__);
    return 0;
}
int myclose(struct inode *node, struct file *file_t)
{
    printk("%s %s %d\n", __FILE__, __func__, __LINE__);
    return 0;
}

// 操作方式结构体
struct file_operations fops = {
    .open = myopen,
    .read = myread,
    .write = mywrite,
    .release = myclose,
};

static int __init ledDrv_init(void)
{
    printk("%s %s %d\n", __FILE__, __func__, __LINE__);
    // 注册字符设备
    major = register_chrdev(major, NAME, &fops);
    // 容错判断
    if (major < 0)
    {
        printk("chrdev_register err.\n");
        return -EINVAL;
    }
        printk("%d\n", major);
    return 0;
}
static void __exit ledDrv_exit(void)
{
    printk(KERN_ERR "%s %s %d\n", __FILE__, __func__, __LINE__);
    unregister_chrdev(major, NAME);
}

module_init(ledDrv_init);
module_exit(ledDrv_exit);
MODULE_LICENSE("GPL");