/*
 * @Author: zyw zhangyuanwei1130@163.com
 * @Date: 2023-12-20 15:31:45
 * @LastEditors: zyw zhangyuanwei1130@163.com
 * @LastEditTime: 2023-12-20 15:48:08
 * @FilePath: /driver/led_drv.c
 * @Description:
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/uaccess.h>
// 定义设备号
#define NAME "chrdev_led"

int major = 0; // 保存主设备号
//开辟空间读写数据
char kbuf[32];
char kbuf_r[32]="recive message";
//定义宏保存物理地址基地址
#define RED_BASE 0xc001a000

//定义指针保存映射后的虚拟地址首地址
unsigned int *red_addr=NULL;

// 函数实现
int myopen(struct inode *node, struct file *file_t)
{
    printk("%s %s %d\n", __FILE__, __func__, __LINE__);
    return 0;
}
ssize_t myread(struct file *file_t, char __user *ubuf, size_t size, loff_t *off_t)
{//将内核空间数据拷贝到用户空间
		//防止越界
		if(sizeof(kbuf_r)<size)
		{
				size=sizeof(kbuf_r);
		}
		//容错判断
		if(copy_to_user(ubuf,kbuf_r,size)!=0)
		{
				printk("copy_to_user err\n");
				return -EINVAL;
		}
    printk("%s %s %d\n", __FILE__, __func__, __LINE__);
    return 0;
}
ssize_t mywrite(struct file *file_t, const char __user *ubuf, size_t size, loff_t *off_t)
{//将用户空间数据拷贝到内核空间
		//防止越界
		if(sizeof(kbuf)<size)
		{
				size=sizeof(kbuf);
		}
		//容错判断
		if(copy_to_user(kbuf,ubuf,size)!=0)
		{
				printk("copy_from_user err\n");
				return -EINVAL;
		}
		printk("kbuf:%s",kbuf);
    printk("%s %s %d\n", __FILE__, __func__, __LINE__);
	//对接收到的消息进行处理
	if(buf[0]==1)
	{
		*red_addr |=(1<<28);
	}else if(kbuf[0]==0)
	{
		*red_addr &=(~(1<<28));
	}
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
    printk("%d\n", major);//打印系统分配的设备号
	//建立虚拟地址和物理地址之间的映射关系
	red_addr=(unsigned int *)ioremap(RED_BASE,40);
	//容错判断
	if(red_addr==NULL)
	{	
        printk("ioremap err.\n");
        return -EINVAL;
	}
	*(red_addr+9)&=(~(3<<24));//选择GPIOA28功能
	*(red_addr+1)|=(1<<28);//选择输出使能
	*red_addr &=(~(1<<28));//红灯关闭
    return 0;
}
static void __exit ledDrv_exit(void)
{
    printk(KERN_ERR "%s %s %d\n", __FILE__, __func__, __LINE__);
	//取消映射
	iounmap(red_addr);
	//注销字符设备驱动
    unregister_chrdev(major, NAME);
}

module_init(ledDrv_init);
module_exit(ledDrv_exit);
MODULE_LICENSE("GPL");
