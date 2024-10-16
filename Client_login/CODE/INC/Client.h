#ifndef __CLIENT__H_
#define __CLIENT__H_

#pragma pack(push, 1) // 保存当前对齐设置，并设置为 1 字节对齐
typedef struct bag_data
{
  char head[4];
  int length;
  char data[1024];
} bag_data;
#pragma pack(pop) // 恢复之前的对齐设置

bag_data *makebag(char *tobu, int length, char *data);

int RecvShujubao( void *data, int len);

int socket_connect(const char *host, const char *port);

int sendfile(char *filename);

void sendbag(char *head, int length, char *data);

int Client(void);

#endif