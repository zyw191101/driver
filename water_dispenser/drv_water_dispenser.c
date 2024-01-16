/*
 * @Author: zyw zhangyuanwei1130@163.com
 * @Date: 2023-12-20 15:31:45
 * @LastEditors: zyw zhangyuanwei1130@163.com
 * @LastEditTime: 2023-12-26 19:49:06
 * @FilePath: /driver/led_drv.c
 * @Description:
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <linux/interrupt.h>

// 设置GPIOno号
#define GPIONO(m, n) (m * 32 + n) // 宏定义gpiono
#define GPIO_NOB8 GPIONO(1, 8)
#define GPIO_NOB16 GPIONO(1, 16)
#define GPIO_NOA28 GPIONO(0, 28)
#define GPIO_NOE13 GPIONO(4, 13)

int gpiono[] = {GPIO_NOB8, GPIO_NOB16}; //
char *name[] = {"interrput_b8", "interrupt_b16"};

// 注销中断使用的参数
int i;

// 定时器所需结构体创建
struct timer_list mytimer;

// 定义设备号
#define NAME "water_dispenser"
int major = 0; // 保存主设备号

// 设备号自动化相关结构体指针
struct class *cls = NULL;
struct device *dev = NULL;

// 开辟空间读写数据
int kbuf[32] = {0};
char kbuf_r[32] = "recive message";
// 定义宏保存物理地址基地址
#define RED_BASE 0xc001a000
#define GREEN_BASE 0xc001e000
#define BLUE_BASE 0xc001b000
#define BUZ_BASE 0xc001c000 // GPIOC14 01= ALT Function1
// 定义指针保存映射后的虚拟地址首地址
unsigned int *red_addr = NULL;
unsigned int *green_addr = NULL;
unsigned int *blue_addr = NULL;
unsigned int *buz_addr = NULL;

// 中断处理函数,按键消抖
irqreturn_t irq_handler_temp(int irq, void *arg)
{
	mod_timer(&mytimer, jiffies+100);
	return IRQ_HANDLED;
}

// a.定时器中断处理函数
irqreturn_t irq_handler(int irq, void *arg)
{
	// 启动定时器
	mod_timer(&mytimer, jiffies + 100);
	if(kbuf[2]!=0){
	if (irq == gpio_to_irq(GPIO_NOB8))
	{
		printk(KERN_ALERT "放水\n");
		*red_addr |= (1 << 28);		 // 红灯亮
		*green_addr &= (~(1 << 13)); // 绿灯灭
		*buz_addr |= (1 << 14);		 // 蜂鸣器响
	}
	if (irq == gpio_to_irq(GPIO_NOB16))
	{
		//*buz_addr |=(0<<14);
	}}
	return IRQ_HANDLED;
}
// a.1定时处理函数
void time_handler(unsigned long data)
{
	int time_buz=0;
	int flag =0;
	int time_water=0;
	// 检测引脚状态，低电平出发中断。闪烁
	if (gpio_get_value(GPIO_NOA28) == 0 && gpio_get_value(GPIO_NOE13) == 0)
	{
		// printk(KERN_ALERT "红灯开\n");
		*red_addr |= (1 << 28);
	}
	else
	{
		// printk(KERN_ALERT "红灯关\n");
		*red_addr &= (~(1 << 28));
	}
	//按键控制水的开关
	if (gpio_to_irq(GPIO_NOB8))
	{
		if(kubf[2]){
		printk(KERN_ALERT "放水\n");
		*red_addr |= (1 << 28);		 // 红灯亮
		*green_addr &= (~(1 << 13)); // 绿灯灭
		//*buz_addr |= (1 << 14);		 // 蜂鸣器响
		mod_timer(&mytimer, jiffies+kbuf[2]*1000);}
		
	}
	if (irq == gpio_to_irq(GPIO_NOB16))
	{
		
	}

}

// 函数实现
int myopen(struct inode *node, struct file *file_t)
{
	printk("%s %s %d\n", __FILE__, __func__, __LINE__);
	return 0;
}
ssize_t myread(struct file *file_t, char __user *ubuf, size_t size, loff_t *off_t)
{ // 将内核空间数据拷贝到用户空间
	// 防止越界
	if (sizeof(kbuf_r) < size)
	{
		size = sizeof(kbuf_r);
	}
	// 容错判断
	if (copy_to_user(ubuf, kbuf_r, size) != 0)
	{
		printk("copy_to_user err\n");
		return -EINVAL;
	}
	printk("%s %s %d\n", __FILE__, __func__, __LINE__);
	return 0;
}
ssize_t mywrite(struct file *file_t, const char __user *ubuf, size_t size, loff_t *off_t)
{ // 将用户空间数据拷贝到内核空间
	// 防止越界
	if (sizeof(kbuf) < size)
	{
		size = sizeof(kbuf);
	}
	// 容错判断
	if (copy_from_user(kbuf, (int *)ubuf, size) != 0)
	{
		printk("copy_from_user err\n");
		return -EINVAL;
	}
	printk(KERN_ALERT "kbuf:%d\n", kbuf[4]);

	printk("%s %s %d\n", __FILE__, __func__, __LINE__);
	// 对接收到的消息进行处理
	// 红灯
	if (kbuf[4] == 1)
	{
		*green_addr |= (1 << 13);
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
static int __init water_dispenser_init(void)
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
	printk("%d\n", major); // 打印系统分配的设备号
	// 建立虚拟地址和物理地址之间的映射关系
	red_addr = (unsigned int *)ioremap(RED_BASE, 40);
	// 容错判断
	if (red_addr == NULL)
	{
		printk("red ioremap err.\n");
		return -EINVAL;
	}
	// 红灯初始化
	*(red_addr + 9) &= (~(3 << 24)); // 选择GPIOA28功能
	*(red_addr + 1) |= (1 << 28);	 // 选择输出使能
	*red_addr &= (~(1 << 28));		 // 红灯关闭

	green_addr = (unsigned int *)ioremap(GREEN_BASE, 40);
	// 容错判断
	if (green_addr == NULL)
	{
		printk("green ioremap err.\n");
		return -EINVAL;
	}
	// 绿灯初始化
	*(green_addr + 8) &= (~(3 << 26)); // 选择GPIOE13功能
	*(green_addr + 1) |= (1 << 13);	   // 选择输出使能
	*green_addr &= (~(1 << 13));	   // 绿灯关闭

	blue_addr = (unsigned int *)ioremap(BLUE_BASE, 40);
	// 容错判断
	if (blue_addr == NULL)
	{
		printk("blue ioremap err.\n");
		return -EINVAL;
	}
	// 蓝灯初始化
	*(blue_addr + 8) &= (~(3 << 24));
	*(blue_addr + 8) |= (1 << 25); // 选择GPIOB12功能
	// 1<<25等价于2<<24
	*(blue_addr + 1) |= (1 << 12); // 选择输出使能
	*blue_addr &= (~(1 << 12));	   // 蓝灯关闭

	// 蜂鸣器
	buz_addr = (unsigned int *)ioremap(BUZ_BASE, 40);
	// 容错判断
	if (buz_addr == NULL)
	{
		printk("blue ioremap err.\n");
		return -EINVAL;
	}
	*(buz_addr + 8) &= (~(3 << 28)); //[29:28]
	*(buz_addr + 8) |= (1 << 28);
	*(buz_addr + 1) |= (1 << 14);
	*buz_addr &= (~(1 << 14));

	// 创建设备节点
	// 1.提交目录信息
	cls = class_create(THIS_MODULE, NAME);
	// 容错判断
	if (IS_ERR(cls))
	{
		printk("class create err.\n");
		return -EINVAL;
	}
	// 2.提交文件信息
	dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "water_dispenser");
	if (IS_ERR(dev))
	{
		printk("device create err.\n");
		return -EINVAL;
	}
	// a.2初始化定时器
	init_timer(&mytimer);			 // 内核自动初始化定时器
	mytimer.expires = jiffies + 100; // 定时时长（毫秒）
	mytimer.function = time_handler;
	mytimer.data = 0;
	// a.3添加定时器
	add_timer(&mytimer);
	// 注册中断
	for (i = 0; i < sizeof(gpiono) / sizeof(int); i++)
	{
		if (request_irq(gpio_to_irq(gpiono[i]), irq_handler, IRQF_TRIGGER_FALLING, name[i], NULL) != 0)
		{
			printk("%s request_irq err\n", name[i]);
			return -EINVAL;
		}
	}

	return 0;
}
static void __exit water_dispenser_exit(void)
{
	printk(KERN_ERR "%s %s %d\n", __FILE__, __func__, __LINE__);
	// 自动删除设备节点
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	// 注销中断
	for (i = 0; i < sizeof(gpiono) / sizeof(int); i++)
	{
		free_irq(gpio_to_irq(gpiono[i]), NULL);
	}
	// 取消映射
	iounmap(red_addr);
	iounmap(green_addr);
	iounmap(blue_addr);
	iounmap(buz_addr);

	// 注销字符设备驱动
	unregister_chrdev(major, NAME);
}

module_init(water_dispenser_init);
module_exit(water_dispenser_exit);
MODULE_LICENSE("GPL");
