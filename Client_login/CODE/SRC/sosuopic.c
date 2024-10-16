#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sosuopic.h"
#include "BmpPicture.h"
#include "jpg.h"
#include "link.h"

/*
 *@brief 搜索/source文件夹下的所有文件,把发现的bmp图片和jpeg图片进行存储
 *@param filepathname 源文件夹路径
 *@return 成功返回0 失败返回-1
 *@note 这个函数的实现比较简单，主要是使用readdir()函数递归遍历源文件夹
 */
int sosuopic(const char *filepathname, void *L)
{
  if (!L)
    return -1;
  linkhead *l = L;
  DIR *dp = opendir(filepathname);
  if (dp == NULL)
  {
    perror("opendir error");
    return -1;
  }
  struct dirent *dirt;
  while (dirt = readdir(dp))
  {
    if (!strcmp(dirt->d_name, ".") || !strcmp(dirt->d_name, ".."))
      continue;
    char *pathname = malloc(sizeof(filepathname) + sizeof(dirt->d_name));
    sprintf(pathname, "%s/%s", filepathname, dirt->d_name);
    if (dirt->d_type == DT_REG) // 普通文件
    {
      if (!strcmp(pathname + strlen(pathname) - 4, ".bmp"))
      {
        addpathname(l, pathname);
        readBmp(pathname, l->rear);           // 写入缓存
        printf("路径%s加载成功\n", pathname); // 测试成功
      }
      else if (!strcmp(pathname + strlen(pathname) - 4, ".jpg") || !strcmp(pathname + strlen(pathname) - 5, ".jpeg"))
      {
        addpathname(l, pathname);
        readjpg(pathname, l->rear);           // 写入缓存
        printf("路径%s加载成功\n", pathname); // 测试成功
      }
    }
    else if (dirt->d_type == DT_DIR) // 如果是一个目录
    {
      sosuopic(pathname, l);
    }
  }
  return 0;
}