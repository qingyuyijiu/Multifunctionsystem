// 这是tcp客户端
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/select.h>
#include "Client.h"
#include "login.h"

char *filename = NULL; // 存放客户端要下载的文件名
int sockfd = -1;

/*
 *@brief 打包
 *@param len 要发送的数据长度
 *@param data 要发送的数据
 */
bag_data *makebag(char *head, int length, char *data)
{
    static bag_data filedata;
    if (length > 1024)
    {
        length = 1024;
    }
    memset(&filedata, 0x00, sizeof(bag_data));
    strncpy(filedata.head, head, 4);
    filedata.length = length;
    memcpy(filedata.data, data, length);
    return &filedata;
}

/*
 *@brief 发包
 *@param sockfd，连接套接字
 */
void sendbag(char *head, int length, char *data)
{
    write(sockfd, makebag(head, length, data), sizeof(bag_data));
}

/*
 *@brief 判断接收包是否完整
 *@param sockfd，连接套接字
 *@param data 接收到的包的数据
 *@param len 需要接收到的包的长度
 */
int RecvShujubao(void *data, int len)
{
    unsigned char *recvdata = (unsigned char *)data;
    int recvlen = 0;
    while (recvlen != len)
    {
        int r = recv(sockfd, recvdata + recvlen, len - recvlen, 0);
        if (-1 == r)
        {
            perror("RecvShujubao error ");
            return false;
        }
        else if (0 == r)
        {
            printf("对方断开了，接不到了\n");
            return false;
        }
        recvlen += r;
    }
    return true; // 接收到指定长度就返回true
}

/*
 *@brief 与服务器建立连接
 *@param host，服务器的ip
 *@param port，服务器的端口
 *@return 成功返回连接套接字，失败返回-1
 */
int socket_connect(const char *host, const char *port)
{
    // 创建套接字        IPV4       流式套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        perror("socket error");
        return 0;
    }
    // 准备服务器的地址
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(atoi(port));
    sa.sin_addr.s_addr = inet_addr(host);
    // 连接服务器
    int r = connect(sockfd, (const struct sockaddr *)&sa, sizeof(struct sockaddr_in));
    if (-1 == r)
    {
        perror("connect error");
        return 0;
    }
    return sockfd;
}

/*
 *@brief 发送文件
 *@param filename 要发送的文件名
 */
int sendfile(char *filename)
{
    unsigned char readbuf[1024];
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("open hehe.pcm error");
        return -1;
    }
    // 先发送文件大小
    fseek(fp, 0X00, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0X00, SEEK_SET);
    char buf[16] = {0};
    sprintf(buf, "%d", size);
    sendbag("fils", strlen(buf), buf);
    int sendsize = 0;
    int bag = 0;
    while (!feof(fp))
    {
        int r = fread(readbuf, 1, 1024, fp);
        sendsize += r;
        // 将读到的内容发送出去
        sendbag("file", r, readbuf);
        // printf("第%d次 head:%s,length:%d\n", bag, "file", r);
        bag++;
    }
    sendbag("filo", strlen("ok"), "ok");
    printf("发送文件完毕,发送大小%d\n", sendsize);
    printf("发包次数%d\n", bag);
    fclose(fp);
}

int Client(void)
{
    while (!socket_connect("172.90.0.166", "8888")) // 3秒时间连接
    {
        int num = 0;
        num++;
        printf("正在连接服务器\n");
        sleep(1);
        if (num >= 3)
            return -1;
    }
    printf("服务器连接成功\n");

    return sockfd;
}
