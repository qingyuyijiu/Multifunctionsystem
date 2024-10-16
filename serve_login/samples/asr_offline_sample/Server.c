// tcp服务器
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include "../../include/Server.h"
#include "../../include/mysql.h"
#include "../../include/alsa.h"

int acceptfd = -1; // 服务器套接字

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
 *@brief 创建一个日志文件，并返回文件指针
 *@param ip 客户端的ip地址
 */
void *touchclientfille(char *ip)
{
    char *fillname = malloc(strlen(ip) + 10);
    sprintf(fillname, "UserLog/%s.log", ip);
    printf("日志文件:%s\n", fillname);
    FILE *fd = fopen(fillname, "a+"); // 追加打开
    if (fd == NULL)
    {
        perror("打开日志文件失败");
        return NULL;
    }
    printf("为此用户准备的日志文件路径为:%s,准备记录\n", fillname);
    return fd;
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
        int r = recv(acceptfd, recvdata + recvlen, len - recvlen, 0);
        if (-1 == r)
        {
            perror("RecvShujubao error ");
            return false;
        }
        else if (0 == r)
        {
            printf("%d 断开连接\n", acceptfd);
            return false;
        }
        recvlen += r;
    }
    return true; // 接收到指定长度就返回true
}

/*
 *@brief 记录日志
 *@param log 要写入日志的内容
 *@note 记录用户的ip地址、端口、发送的消息和发送消息的时间
 */
void WriteLog(void *fp, void *arg, void *log)
{
    char tiembuf[144]; // 记录到日志中的时间
    FILE *logfd = fp;
    sever_data *data = arg;
    time_t t = time(NULL);                                                        // 时间
    struct tm *lt = localtime(&t);                                                // 用一个结构体接收
    strftime(tiembuf, sizeof(tiembuf), "%Y-%m-%d %H:%M:%S", lt);                  // 把时间写入到一个字符串
    fprintf(logfd, "%s %s %d: %s\n", tiembuf, data->ip, data->port, (char *)log); // 记录日志
}

void *RecvFile(int filesize)
{
    printf("文件大小为 %d\n", filesize);
    void *ptr = malloc(filesize);
    // 循环接收信息
    FILE *pcmfp = fopen("./wav/hehe.pcm", "wb");
    int recvsize = 0;
    int bag = 0;
    while (1)
    {
        bag_data data;
        memset(&data, 0, sizeof(bag_data));
        RecvShujubao(&data, sizeof(bag_data));
        bag++;
        // printf("第%d次 head:%s,length:%d\n", bag, data.head, data.length);
        recvsize += data.length;
        if (!strncmp(data.head, "filo", 4))
        {
            if (!strncmp(data.data, "ok", 2))
            {
                printf("接收文件头成功\n");
                if (recvsize >= filesize) // 完毕了
                {
                    printf("接收文件内容完毕\n");
                    break;
                }
                else
                {
                    bag--;
                    printf("接收文件有误 长度%d,收包%d\n", recvsize, bag);
                    break;
                }
            }
        }
        if (!strncmp(data.head, "file", 4))
        {
            strcat(ptr, data.data);
            fwrite(data.data, 1, data.length, pcmfp);
        }
    }
    fclose(pcmfp);
    return ptr;
}

/*
 *@brief 服务器线程服务一个客户端
 *@param arg 这个里面必须将这个连接套接字拿到，里面的id为日志名
 */
void *ServerClient(void *arg)
{
    pthread_detach(pthread_self());
    sever_data *cliendata = arg;                   // 这个里面必须将这个连接套接字拿到
    acceptfd = *(cliendata->accept);               // 这个里面必须将这个连接套接字拿到
    FILE *logfp = touchclientfille(cliendata->ip); // 创建日志文
    int alsainit = 0;
    while (1)
    {
        char account[14] = {0};
        char phone[14] = {0};
        char password[14] = {0};
        char logmessage[1024] = {0};
        char sendmsg[14] = {0};
        bag_data data;
        memset(&data, 0, sizeof(bag_data));
        if (!RecvShujubao(&data, sizeof(bag_data)))
        {
            printf("该用户的日志已记录\n");
            break;
        }
        if (&data != NULL)
        {
            printf("\t\t客户端发的消息:head:%s,length:%d,data:%s\n", data.head, data.length, data.data);
            if (!strncmp(data.head, "zaho", 4)) // 此处为登陆
            {
                strcpy(account, data.data);

                memset(&data, 0, sizeof(bag_data));
                RecvShujubao(&data, sizeof(bag_data));

                if (&data != NULL)
                    printf("\t\t客户端发的消息:head:%s,length:%d,data:%s\n", data.head, data.length, data.data);
                strcpy(password, data.data);

                int r = Cmp_Accountpassword(account, password);
                // printf("account: %s,password: %s r:%d\n", account, password, r);
                sprintf(logmessage, "尝试登陆账号:%s", account);
                if (0 == r)
                {
                    sprintf(sendmsg, "%s", "ok");
                    printf("密码正确,登录成功\n");
                    strcat(logmessage, " 登录成功");
                }
                else if (-1 == r)
                {
                    sprintf(sendmsg, "%s", "err");
                    printf("没有此账号\n");
                    strcat(logmessage, " 登录失败,没有此账号");
                }
                else
                {
                    sprintf(sendmsg, "%s", "err");
                    printf("密码错误\n");
                    strcat(logmessage, " 登录失败,密码错误");
                }
                ServerSendbag("zaho", strlen(sendmsg), sendmsg);
                printf("logmessage:%s\n", logmessage);
                WriteLog(logfp, cliendata, logmessage); // 记录日志
            }
            else if (!strncmp(data.head, "zuce", 4)) // 此处为注册
            {
                strcpy(phone, data.data);
                memset(&data, 0, sizeof(bag_data));
                RecvShujubao(&data, sizeof(bag_data));
                if (&data != NULL)
                    printf("\t\t客户端发的消息:head:%s,length:%d,data:%s\n", data.head, data.length, data.data);
                strcpy(password, data.data);
                char *randaccount = User_Add(phone, password);
                printf("用户注册成功,账号为:%s\n", randaccount);
                ServerSendbag("zuce", strlen(randaccount), randaccount);
                sprintf(logmessage, "注册了账号,账号为:%s", randaccount);
                WriteLog(logfp, cliendata, logmessage); // 记录日志
            }
            else if (!strncmp(data.head, "fils", 4)) // 此处客户端发送文件
            {
                int filesize = 0;
                filesize = atoi(data.data);
                char *data = RecvFile(filesize);
                printf("初始化语音识别\n");
                alsa_init(NULL);
                printf("识别完成\n");
            }
        }
    }
    close(acceptfd);
    fclose(logfp);
    free(cliendata->ip);
    free(cliendata);
}

/*
 *@brief 创建套接字并绑定ip和端口
 */
int socket_bind(const char *host, const char *port)
{
    // 创建套接字        IPV4       流式套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        perror("socket error");
        return -1;
    }

    struct sockaddr_in sa;   // 绑定ip和端口
    sa.sin_family = AF_INET; // 绑定服务器地址
    sa.sin_port = htons(atoi(port));
    sa.sin_addr.s_addr = inet_addr(host);
    int r = bind(sockfd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in));
    if (-1 == r)
    {
        perror("bind error");
        return -2;
    }
    listen(sockfd, 10); // 建立监听队列
    return sockfd;
}

/*
 *@brief 与客户端建立连接
 */
int Tcp_connect(int sockfd)
{
    struct sockaddr_in sa1;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    // 循环等待客户端的连接
    while (1)
    {
        // acceptfd = malloc(4); // 暂时只能服务一个客户端
        acceptfd = accept(sockfd, (struct sockaddr *)&sa1, &addrlen);
        if (-1 == acceptfd)
        {
            perror("连接失败，走人\n");
            break;
        }
        char *ip = inet_ntoa(sa1.sin_addr);
        int port = ntohs(sa1.sin_port);
        printf("ip为:%s的客户端已与服务器建立连接, 端口为:%d\n", ip, port);
        sever_data *data = malloc(sizeof(sever_data)); // 创建一个结构体接收要发送的文件
        data->port = port;
        data->ip = malloc(strlen(ip) + 1);
        strcpy(data->ip, ip); // 保存客户端的ip地址
        data->accept = &acceptfd;
        pthread_t thread; // 开一个线程去服务这个客户
        if (pthread_create(&thread, NULL, ServerClient, (void *)data) != 0)
        {
            perror("开辟线程失败\n");
            break;
        }
    }
    return 0;
}

/*
 *@brief 发包
 *@param sockfd，连接套接字
 */
void ServerSendbag(char *head, int length, char *data)
{
    write(acceptfd, makebag(head, length, data), sizeof(bag_data));
}

// 自己一定要分功能 不能全部弄到main里面
void *Server(void *arg)
{
    // pthread_detach(pthread_self());
    printf("服务器启动成功,ok了\n");
    int sockfd = socket_bind("172.90.0.166", "8888");
    Tcp_connect(sockfd);
    close(sockfd);
    return 0;
}
