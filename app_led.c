/*
 * @Author: zyw zhangyuanwei1130@163.com
 * @Date: 2023-12-20 09:48:51
 * @LastEditors: zyw zhangyuanwei1130@163.com
 * @LastEditTime: 2023-12-20 10:32:28
 * @FilePath: /driver/led_app.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
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
	char buf[32];
	read(fd,buf,sizeof(buf));
	write(fd,buf,sizeof(buf));
	read(fd,buf,sizeof(buf));

	//关闭文件
	close(fd);
    return 0;
}
