/*
 * @Author: zyw zhangyuanwei1130@163.com
 * @Date: 2023-12-22 11:21:59
 * @LastEditors: zyw zhangyuanwei1130@163.com
 * @LastEditTime: 2023-12-25 19:04:10
 * @FilePath: /driver/water_dispenser/app.c
 * @Description:
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// 定义消息作为全局变量(结构体感觉也可以)
int message[32];

// 数据处理函数
int data_packet(void)
{
    // 1.检测终端输入数据
    // 1.1包头包尾的数据处理功能（输入格式：0x55(包头)0x01(id)0x02(金额2元)0xff(包尾) ）
    // 0x550x010x020xff (id为1价格为2)
    int head = 0x55;
    int id, money;
    printf("请输入id\n");
    scanf("%d", &id);
    printf("请输入金额\n");
    scanf("%d", &money);
    int tail = 0xff;
    // 字符拼接
    message[0] = head;
    message[1] = id;
    message[2] = money;
    message[3] = tail;

    // for (int i = 0; i <4; i++)
    // {
    //     printf("%#x", message[i]);
    // }
    // 2.文件保存
    // 2.1打开文件
    FILE *file = fopen("data.txt", "a+");
    // 2.2容错判断
    if (file == NULL)
    {
        printf("Unable to open the file.\n");
        return 1;
    }
    // 2.3往文件里面写数据
    fprintf(file, "用户id:%d 金额:%d\n", id, money);
    // 2.4关闭文件指针
    fclose(file);
    return 0;
}

// 数据传输函数
int data_transmission()
{
    // 打开字符设备文件
    int fd = open("/dev/water_dispenser", O_RDWR);
    // 容错判断
    if (fd < 0)
    {
        perror("open file err\n");
        return -1;
    }
    write(fd, message, sizeof(message));
    // 关闭文件
    close(fd);
    return 0;
}

int main(int argc, char const *argv[])
{
    //应用安装成功标志
    message[4] = 1;
    data_transmission();
    while (1)
    {
        data_packet();
        data_transmission();
    }

    return 0;
}
