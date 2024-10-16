#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/input.h>
#include "lcd.h"
#include "BmpPicture.h"
#include "piviv.h"

static int piviv_fd = -1;

/*
 *@brief 初始化画布
 */
int initCancas()
{
  BmpPictureDisplay("/source/backgroundpic/piviv.bmp");
  drawyuan(750, 400, 20, 0xffa07a); // 10为最大画笔大小，后面是图片颜色
  for (int x = 0; x < 800; x++)
    for (int y = 0; y <= 50; y++)
    {
      int t = x / 50;
      DrawPoint(x, y, 0xffffff / 13 * t);
    }
}

/*
 *@brief 清空画布
 */
int cleanCancas()
{
  for (int i = 0; i <= 700; i++)
    for (int j = 50 - 1; j < 480; j++)
    {
      DrawPoint(i, j, 0xffffff);
    }
}

/*
 *@brief 打开画板
 */
int pivivOpen()
{
  piviv_fd = open("/dev/input/event0", O_RDWR);
  if (-1 == piviv_fd)
  {
    perror("open event0 error");
    exit(6);
  }
}

/*
 *@brief 绘制
 */
int piviv()
{

  int x, y;     // 坐标
  int flag = 0; // 光滑
  struct input_event ev;
  int x0 = -1, y0 = -1;
  int x1 = -1, y1 = -1;
  int static bitold = -1;  // 判断画笔大小是否改变
  int static clorold = -1; // 判断画笔颜色是否改变
  int bit = 1;             // 画笔大小，初始为1
  int color = 0;           // 画笔颜色，初始为黑色
  initCancas();            // 初始化画布
  pivivOpen();
  printf("进入画板\n");
  while (1)
  {
    int r = read(piviv_fd, &ev, sizeof(ev));
    if (ev.type == EV_ABS)
    {
      if (ev.code == ABS_X)
      {
        x1 = ev.value;
      }
      else if (ev.code == ABS_Y)
      {
        y1 = ev.value;
      }
      x = x1 * 800 / 1024;
      y = y1 * 480 / 595;
      if (x0 != -1 && y0 != -1 && x != -1 && y != -1) // 变光滑
      {
        if (x0 == x || y0 == y)
        {
          if (flag == 1)
          {
            if (x0 == x)
            {
              y = (y0 + y) / 2;
            }
            else if (y0 == y)
            {
              x = (x0 + x) / 2;
            }
            flag = 1;
          }
          else
          {
            continue;
          }
        }
        flag = 0;
        if (x < 700 - bit && y > 50 + bit)
          DrawLine(x0, y0, x, y, bit, color);
        else // 画笔越界就重置
        {
          x0 = -1;
          y0 = -1;
        }
        if (x < 700 - bit && y > 50 + bit) // 限制画布大小,只有在这个范围内才有起始点，才能画
        {
          x0 = x;
          y0 = y;
        }
      }
    }
    if (ev.type == 0x01 && ev.code == BTN_TOUCH && ev.value == 0) // 抬手
    {
      if (x0 == x && y0 == y) // 点击，只画一个点
      {
        if (x < 700 - bit && y > 50 + bit)
          DrawLine(x0, y0, x, y, bit, color);
      }
      x0 = -1, y0 = -1;
      if (x > 700 && x < 800 && y < 90 && y > 50) // 清空画布
      {
        cleanCancas();
      }
      else if (x > 700 && x < 800 && y > 440 && y < 495) // 退出程序
      {
        pivivReturn();
        break;
      }
      else if (x > 700 && x < 800 && y < 225 && y > 90) // 变大画笔，最大为10
      {
        if (bit < 20)
          bit++;
      }
      else if (x > 700 && x < 800 && y < 360 && y > 225) // 变小画笔，最小为1
      {
        if (bit > 1)
          bit--;
      }
      else if (y > 0 && y < 50) // 点击获取画笔颜色
      {
        int t = x / 50;
        color = 0xffffff / 13 * t;
      }
      if (bitold != bit || color != clorold)
      {
        drawyuan(750, 400, 20, 0xffa07a); // 10为最大画笔大小，后面是图片颜色
        drawyuan(750, 400, bit, color);
        bitold = bit;
        clorold = color;
      }
    }
    else if (0x01 == ev.type && BTN_TOUCH == ev.code && 0x01 == ev.value)
    {
      x0 = x;
      y0 = y;
    }
  }
}

/*
 *@brief 退出画板
 */
int pivivReturn()
{
  close(piviv_fd);
}