#ifndef __SERVER__H_
#define __SERVER__H_

typedef struct sever_data // 为客户端线程准备的数据结构，
{
  int *accept; // 连接套接字
  char *ip;    // 客户端ip
  int port;    // 客户端端口
} sever_data;

#pragma pack(push, 1) // 保存当前对齐设置，并设置为 1 字节对齐
typedef struct bag_data
{
  char head[4];
  int length;
  char data[1024];
} bag_data;
#pragma pack(pop) // 恢复之前的对齐设置

// int sosuofile(const char *filepathname, void *L);

void *Server(void *arg);

void *touchclientfille(char *ip);

void ServerSendbag(char *head, int length, char *data);

void WriteLog(void *fp, void *arg, void *log);

void *ServerClient(void *arg);

int socket_bind(const char *host, const char *port);

int Tcp_connect(int sockfd);

int RecvShujubao(void *data, int len);

void *RecvFile(int filesize);

#endif