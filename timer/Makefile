#编译器选择
KERNEL_PATH=/home/zyw/toolchain/kernel-3.4.39
#KERNEL_PATH=/lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)
all:
	make -C $(KERNEL_PATH) M=$(PWD) modules
	#-C 路径:指定到切换到那个路径 执行make modules命令
	# M=路径:指定需要编译的驱动代码所在的路径
.PHONY:clean
clean:
	make -C $(KERNEL_PATH) M=$(PWD) clean
#obj-m +=hello.o
#obj-m +=prc_1218_evening.o
obj-m +=timer.o#与要编译的文件名字相同
#obj-m += demo.o
#demo-y += hello.o add.o#-y作用:将hello.o add.o 放到demo.o中
#obj-m += demo.o
#obj-m内核Makefile调用需要的参数。

