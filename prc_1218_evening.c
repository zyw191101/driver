//1.byte类型如何使用（传递参数用ascii）
//2.如何给一个指针传递字符串
#include<linux/init.h>
#include<linux/module.h>
int a=10;
module_param(a,int,0775);
MODULE_PARM_DESC(a,"this is lcd light");

short b =11;
module_param(b,short,0664);
MODULE_PARM_DESC(b,"this is rgb");

char ch ='c';
module_param(ch,byte,0775);
MODULE_PARM_DESC(ch,"this is ch val");

char *p =NULL;
module_param(p,charp,0664);
MODULE_PARM_DESC(p,"this is ch *p");

static int __init prc_init(void){
printk("a is val = %d\n",a);
printk("b is val = %d\n",b);
printk("c is val = %c\n",ch);
printk("p is val = %s\n",p);
return 0;
}

static void __exit prc_exit(void){
}
module_init(prc_init);
module_exit(prc_exit);
MODULE_LICENSE("GPL");
