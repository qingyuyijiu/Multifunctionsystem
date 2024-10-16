#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "albumface.h"
#include "link.h"
#include "lcd.h"
#include "touch.h"
#include "BmpPicture.h"
#include "jpg.h"

static int flag = 0;  // 没进入相册时需要置0
static int slip = -1; // 没进入相册时需要置-1，0向下滑动，1向上滑动

/*
 *@brief 把flag置0,slip置为-1
 */
void set_flag()
{
  slip = -1;
  flag = 0;
}

/*
 *@brief 相册界面设计，同时显示4张相册中的图片，向下滑动切换图片，点击图片进入单图模式
 *@param L保存图片信息的循环单链表的头节点
 *@return 返回值为保存图片信息节点
 *@note 第一次进入时getnext()传入的flag必须为0，否则会报断错误
 *@author xwp
 */
void *AlbumInterface(void *L)
{
  if (!L)
  {
    printf("没有搜索到图片\n");
    return NULL;
  }
  TouchDirection touch;
  linknode *node[4];
  linknode *p;
  int *rgb;
  int x, y;
  while (1) // 多图模式到单图模式的切换在在里面进行
  {

    if (flag == 1) // flag==1表示已经进入相册，可以进行点击或者退出等操作
    {
      touch = GetTouchDirection(&x, &y);
      if (touch == Touchreturn) // 返回界面,优先级最高
      {
        return NULL;
      }
      else if (touch == TouchChick)
      {
        printf("点击");
        if (x > 0 && x < node[0]->w / 2 && y > 0 && y < node[0]->h / 2) // 返回存储第一张图片图片的节点,位置0-x/2 0-y/2
        {
          printf("图1 %s\n", node[0]->data);
          onepicmode(node[0]);
        }
        else if (x > node[0]->w / 2 && x < node[0]->w && y > 0 && y < node[0]->h / 2) // 返回存储第二张图片的节点
        {
          printf("图2 %s\n", node[1]->data);
          onepicmode(node[1]);
        }
        else if (x > 0 && x < node[0]->w / 2 && y > node[0]->h / 2 && y < node[0]->h) // 返回存储第三张图片的节点
        {
          printf("图3 %s\n", node[2]->data);
          onepicmode(node[2]);
        }
        else if (x > node[0]->w / 2 && x < node[0]->w && y > node[0]->h / 2 && y < node[0]->h) // 返回存储第四张图片的节点
        {
          printf("图4 %s\n", node[3]->data);
          onepicmode(node[3]);
        }
      }
    }
    if (touch == TouchUp || flag == 0) // 进入相册会从此处加载第一张图片开始显示,上滑切换更新相册中的图片
    {
      printf("上滑切换相册图片\n");
      if (0 == slip) // 如果之前是下滑需要getnext三次
        for (int i = 0; i < 3; i++)
          getnext(L, flag);
      slip = 1;                   // 设为上滑标志
      node[0] = getnext(L, flag); // 保存相册中显示的保存第一张图片图片路径的节点,首次进入flag必须要为0
      if (flag == 0)              // 第一次进入相册
      {
        printf("进入相册\n");
        flag = 1;
      }
      node[1] = getnext(L, flag); // 保存相册中显示的保存第二张图片路径的节点
      node[2] = getnext(L, flag); // 保存相册中显示的保存第三张图片路径的节点
      node[3] = getnext(L, flag); // 保存相册中显示的保存第四张图片路径的节点
    }
    else if (touch == TouchDown) // 下滑切换更新相册中的图片
    {
      printf("下滑切换相册图片\n");
      if (1 == slip) // 如果之前是上滑需要getnext三次
        for (int i = 0; i < 3; i++)
          getlast(L, flag);
      slip = 0;                   // 设为下滑标志
      node[3] = getlast(L, flag); // 保存相册中显示的保存第四张图片图片路径的节点,首次进入flag必须要为0
      node[2] = getlast(L, flag); // 保存相册中显示的保存第三张图片路径的节点
      node[1] = getlast(L, flag); // 保存相册中显示的保存第二张图片路径的节点
      node[0] = getlast(L, flag); // 保存相册中显示的保存第一张图片路径的节点
    }

    /*-----------------------------------多图模式(4张)显示相册中的图片-----------------------------------*/
    if (flag == 0)
    {
      printf("进入相册\n");
      flag = 1;
    }
    rgb = node[0]->color;
    if (!strcmp(node[0]->data + strlen(node[0]->data) - 4, ".bmp"))
    {
      printf("第一张图片 %s \n", node[0]->data);
      for (int y = (node[0]->h / 2) - 1; y >= 0; y--)
      {
        for (int x = 0; x < node[0]->w / 2; x++)
        {
          rgb += 2;
          DrawPoint(x, y, *rgb);
        }
        rgb += node[0]->w;
      }
    }
    else
    {
      printf("第一张图片 %s \n", node[0]->data);
      rgb += node[0]->h * node[0]->w - 1;
      for (int y = (node[0]->h / 2) - 1; y >= 0; y--)
      {
        for (int x = 0; x < node[0]->w / 2; x++)
        {
          rgb -= 2;
          DrawPoint(x, y, *rgb);
        }
        rgb -= node[0]->w;
      }
    }

    rgb = node[1]->color;
    if (flag == 0)
    {
      flag = 1;
    }
    if (!strcmp(node[1]->data + strlen(node[1]->data) - 4, ".bmp"))
    {
      printf("第二张图片 %s \n", node[1]->data);
      for (int y = (node[1]->h / 2) - 1; y >= 0; y--)
      {
        for (int x = node[1]->w / 2; x < node[1]->w; x++)
        {
          rgb += 2;
          DrawPoint(x, y, *rgb);
        }
        rgb += node[1]->w;
      }
    }
    else
    {
      printf("第二张图片 %s \n", node[1]->data);
      rgb += node[1]->h * node[1]->w - 1;
      for (int y = (node[1]->h / 2) - 1; y >= 0; y--)
      {
        for (int x = node[1]->w / 2; x < node[1]->w; x++)
        {
          rgb -= 2;
          DrawPoint(x, y, *rgb);
        }
        rgb -= node[1]->w;
      }
    }

    rgb = node[2]->color;
    if (flag == 0)
    {
      flag = 1;
    }
    if (!strcmp(node[2]->data + strlen(node[2]->data) - 4, ".bmp"))
    {
      printf("第三张图片 %s \n", node[2]->data);
      for (int y = node[2]->h - 1; y >= (node[2]->h / 2); y--)
      {
        for (int x = 0; x < node[2]->w / 2; x++)
        {
          rgb += 2;
          DrawPoint(x, y, *rgb);
        }
        rgb += node[2]->w;
      }
    }
    else
    {
      printf("第三张图片 %s \n", node[2]->data);
      rgb += node[2]->h * node[2]->w - 1;
      for (int y = node[2]->h - 1; y >= (node[2]->h / 2); y--)
      {
        for (int x = 0; x < node[2]->w / 2; x++)
        {
          rgb -= 2;
          DrawPoint(x, y, *rgb);
        }
        rgb -= node[2]->w;
      }
    }

    rgb = node[3]->color;
    if (flag == 0)
    {
      flag = 1;
    }
    if (!strcmp(node[3]->data + strlen(node[3]->data) - 4, ".bmp"))
    {
      printf("第四张图片 %s \n", node[3]->data);
      for (int y = node[3]->h - 1; y >= (node[3]->h / 2); y--)
      {
        for (int x = node[3]->w / 2; x < node[3]->w; x++)
        {
          rgb += 2;
          DrawPoint(x, y, *rgb);
        }
        rgb += node[3]->w;
      }
    }
    else
    {
      printf("第四张图片 %s \n", node[3]->data);
      rgb += node[3]->h * node[3]->w - 1;
      for (int y = node[3]->h - 1; y >= (node[3]->h / 2); y--)
      {
        for (int x = node[3]->w / 2; x < node[3]->w; x++)
        {
          rgb -= 2;
          DrawPoint(x, y, *rgb);
        }
        rgb -= node[3]->w;
      }
    }
  }
  return 0;
}

/*
 *@brief 单图模式，点击返回到相册之前的状态
 *@param node保存图片路径信息的节点
 *@note 点击切换图片
 */
void *onepicmode(void *node)
{
  linknode *p = node;
  int x, y;
  int flag = 0;
  int click = -1;
  unsigned char *pathname = p->data;
  TouchDirection touch;
  while (1)
  {
    if (flag == 1)
      touch = GetTouchDirection(&x, &y);
    if (touch == TouchUp || flag == 0)
    {
      if (flag == 0)
        flag = 1;
      else
      {
        p = p->next;
        pathname = p->data;
      }
      if (!strcmp(pathname + strlen(pathname) - 4, ".bmp"))
      {
        DisplayBmp(pathname, p);
      }
      else
      {
        DisplayJpeg(pathname, p);
      }
    }
    else if (touch == TouchDown)
    {
      p = p->last;
      pathname = p->data;
      if (!strcmp(pathname + strlen(pathname) - 4, ".bmp"))
      {
        DisplayBmp(pathname, p);
      }
      else
      {
        DisplayJpeg(pathname, p);
      }
    }
    else if (touch == TouchChick)
    {
      cleanscreen();
      printf("点击\n");
      click++;
      printf("%d\n", click);
      switch (click % 5 + 1)
      {
      case 1:
        displaybaiye(p);
        break;
      case 2:
        displaypicyuanbian(p);
        break;
      case 3:
        displayjianbian(p);
        break;
      case 4:
        displayjianbianxia(p);
        break;
      case 5:
        displaypicyuan(p);
        break;
      default:
        break;
      }
    }
    else if (touch == Touchreturn) // 退出进入多图模式
    {
      printf("返回到相册\n");
      return NULL;
    }
  }
}