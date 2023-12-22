#include<linux/init.h>
#include<linux/module.h>
#include<linux/printk.h>
#include<linux/interrupt.h>
#include<linux/gpio.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define GPIONO(m,n) (m * 32 +n)//宏定义gpiono
#define GPIO_NOB8 GPIONO(1,8)
#define GPIO_NOB16 GPIONO(1,16)

int gpiono[]={GPIO_NOB8,GPIO_NOB16};
char *name[]={"interrput_b8","interrupt_b16"};

int i;
//蜂鸣器
#define BUZ_BASE 0xc001c000
unsigned int *buz_addr=NULL;

//定时器所需结构体创建
struct timer_list mytimer;

//中断处理函数
irqreturn_t irq_handler(int irq,void *arg)
{
	//启动定时器
	mod_timer(&mytimer,jiffies+100);
	if(irq == gpio_to_irq(GPIO_NOB8))
	{
	//*buz_addr &=(~(1<<14));
	}
	if(irq == gpio_to_irq(GPIO_NOB16))
	{
	//*buz_addr |=(0<<14);
	}
	return IRQ_HANDLED;
}
//定时处理函数
void time_handler(unsigned long data)
{
	//检测引脚状态，低电平出发中断
	if(gpio_get_value(GPIO_NOB8)==0)
	{
		printk(KERN_ALERT "++++++++++++++++++\n");
	}
	if(gpio_get_value(GPIO_NOB16)==0)
	{
		printk(KERN_ALERT "-------------------\n");
	}
}
//程序入口
static int __init timer_init(void)//必须interrupt_init！系统里面写好了
{	//初始化定时器
	init_timer(&mytimer);//内核自动初始化定时器
	mytimer.expires=jiffies+100;//定时时长（毫秒）
	mytimer.function=time_handler;
	mytimer.data=0;
	//添加定时器
	add_timer(&mytimer);
	//注册中断
	for(i=0;i<sizeof(gpiono)/sizeof(int);i++)
	{
		if(request_irq(gpio_to_irq(gpiono[i]),irq_handler,IRQF_TRIGGER_FALLING,name[i],NULL)!=0)
		{
				printk("%s request_irq err\n",name[i]);
				return -EINVAL;
		}
	}
	//蜂鸣器初始化
	// buz_addr=(unsigned int *)ioremap(BUZ_BASE,40);
	// //容错判断
	// if(buz_addr==NULL)
	// {	
    //     printk("blue ioremap err.\n");
    //     return -EINVAL;
	// }
	// *(buz_addr+8) &=(~(3<<28));//[29:28] 
	// *(buz_addr+8) |=(1<<28);
	// *(buz_addr+1) |=(1<<14);
	return 0;
}

static void __exit timer_exit(void)
{
	//注销中断
	for(i=0;i<sizeof(gpiono)/sizeof(int);i++)
	{
		free_irq(gpio_to_irq(gpiono[i]),NULL);
	}
	iounmap(buz_addr);
}
module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");





