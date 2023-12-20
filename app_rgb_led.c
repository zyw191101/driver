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
    int fd = open("/dev/ledZyw", O_RDWR);
	//容错判断
	if(fd<0){
	perror("open file err\n");
	return -1;
	}    
	char buf[32]={1,0};
	while(1){
	write(fd,buf,sizeof(buf));
	sleep(1);
	buf[0]=buf[0]?0:1;
	}
	//关闭文件
	close(fd);
    return 0;
}
