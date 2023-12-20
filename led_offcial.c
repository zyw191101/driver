#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

#define LED_GPIO 18 // 假设流水灯的GPIO引脚为18

static dev_t dev_num;
static struct cdev led_cdev;

static int led_open(struct inode *inode, struct file *file)
{
    gpio_request(LED_GPIO, "led_gpio");
    gpio_direction_output(LED_GPIO, 0);
    
    return 0;
}

static ssize_t led_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    int state = gpio_get_value(LED_GPIO);
    char state_str[2];
    
    state_str[0] = state + '0';
    state_str[1] = '\n';
    
    copy_to_user(buf, state_str, 2);
    
    return 2;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    char command;
    
    if (copy_from_user(&command, buf, 1) != 0) {
        return -EFAULT;
    }
    
    if (command == '0') {
        gpio_set_value(LED_GPIO, 0);
    } else if (command == '1') {
        gpio_set_value(LED_GPIO, 1);
    }
    
    return 1;
}

static int led_release(struct inode *inode, struct file *file)
{
    gpio_free(LED_GPIO);
    
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_release,
};

static int __init led_init(void)
{
    int ret;
    
    ret = alloc_chrdev_region(&dev_num, 0, 1, "led_device");
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate device number\n");
        return ret;
    }
    
    cdev_init(&led_cdev, &fops);
    led_cdev.owner = THIS_MODULE;
    
    ret = cdev_add(&led_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "Failed to add character device\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }
    
    return 0;
}

static void __exit led_exit(void)
{
    cdev_del(&led_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("LED Character Device Driver");