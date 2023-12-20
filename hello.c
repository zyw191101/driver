#include <linux/module.h>
#include <linux/init.h>
#include "add.h"
static int __init hello_init(void)
{
	printk("starting\n");
	int a = add(3, 4);
	printk("%d\n", a);
	return 0;
}
static void __exit hello_exit(void)
{
	int a = sub(3, 4);
	printk("%d\n", a);
	printk("ending\n");
}
// 入口：申请资源
module_init(hello_init);
// 出口：释放资源
module_exit(hello_exit);
// 许可证
MODULE_LICENSE("GPL");
