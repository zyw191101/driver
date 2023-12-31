/*
 * @Author: zyw zhangyuanwei1130@163.com
 * @Date: 2023-12-20 09:48:51
 * @LastEditors: zyw zhangyuanwei1130@163.com
 * @LastEditTime: 2023-12-20 15:48:00
 * @FilePath: /driver/led_app.c
 * @Description:
 */
#include <stdio.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
int main(int argc, char const *argv[])
{
	//打开字符设备文件
    int fd = open("./ledZyw", O_RDWR);
	//容错判断
	if(fd<0){
	perror("open file err\n");
	return -1;
	}    
	//对文件进行读写操作
	char buf[32]="app message\n";
	write(fd,buf,sizeof(buf));
	//开辟空间接收内核空间传过来的数据
	char buf_r[32];
	read(fd,buf_r,sizeof(buf_r));
	printf("buf_r:%s\n",buf_r);
	//关闭文件
	close(fd);
    return 0;
}
