/*
 * @Author: zyw zhangyuanwei1130@163.com
 * @Date: 2023-12-21 16:35:26
 * @LastEditors: zyw zhangyuanwei1130@163.com
 * @LastEditTime: 2023-12-22 10:50:37
 * @FilePath: /interrupt/interrupt.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define GPIONO(m, n) (m * 32 + n) // 宏定义gpiono
#define GPIO_NOB8 GPIONO(1, 8)
#define GPIO_NOB16 GPIONO(1, 16)

int gpiono[] = {GPIO_NOB8, GPIO_NOB16};
char *name[] = {"interrput_b8", "interrupt_b16"};

int i;
// 蜂鸣器
#define BUZ_BASE 0xc001c000
unsigned int *buz_addr = NULL;

// 中断处理函数
irqreturn_t irq_handler(int irq, void *arg)
{
	if (irq == gpio_to_irq(GPIO_NOB8))
	{
		printk(KERN_ALERT "++++++++++++++++++\n");
		*buz_addr &= (~(1 << 14));
	}
	if (irq == gpio_to_irq(GPIO_NOB16))
	{
		printk(KERN_ALERT "---------------------\n");
		*buz_addr |= (1 << 14);
	}
	return IRQ_HANDLED;
}

static int __init interrupt_init(void) // 必须interrupt_init！系统里面写好了
{
	// 蜂鸣器初始化
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

static void __exit interrupt_exit(void)
{
	// 注销中断
	for (i = 0; i < sizeof(gpiono) / sizeof(int); i++)
	{
		free_irq(gpio_to_irq(gpiono[i]), NULL);
	}
	iounmap(buz_addr);
}
module_init(interrupt_init);
module_exit(interrupt_exit);
MODULE_LICENSE("GPL");
