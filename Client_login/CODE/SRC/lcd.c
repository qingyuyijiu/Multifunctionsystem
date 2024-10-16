#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "lcd.h"
#include "link.h"

int lcd_fd = -1;
unsigned int *plcd = NULL;
#define r_yuan 8 // 圆每次增加或减小的半径，主要用于图像显示，调整圆形的间隙，注意要合适

/*
 *@brief 初始化LCD屏幕
 *@note 打开/dev/fb0设备文件，文件描述符记录到lcd_fd中，并映射到内存中用plcd指针记录
 *@return void
 *@author xiewanpi
 */
void LcdInit(void)
{
  lcd_fd = open("/dev/fb0", O_RDWR);
  if (-1 == lcd_fd)
  {
    perror("打开设备fb0失败");
    return;
  }
  plcd = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
  if (plcd == MAP_FAILED)
  {
    perror("内存映射失败");
    return;
  }
}

/*
 *@brief 关闭LCD屏
 *@note 释放申请的映射并关闭打开的设备
 */
void close_lcd()
{
  munmap(plcd, 800 * 480 * 4);
  close(lcd_fd);
}

/*
 *@brief 画点函数
 *@param x,y为点的位置坐标，color为画点的颜色
 *@return void
 *@author xiewanpi
 */
void DrawPoint(int x, int y, unsigned int color)
{
  if (x >= 0 && x < 800 && y >= 0 && y < 480)
  {
    *(plcd + 800 * y + x) = color;
  }
}

/*
 *@brief 画矩形
 *@param x0,y0为矩形左上顶点的位置,w和h为矩形的宽和高,rgb是要写入屏幕的颜色
 *@return void
 *@author xiewanpi
 */
void drawjuxing(int x0, int y0, int w, int h, int rgb)
{
  for (int i = 0; i < 800; i++)
  {
    for (int j = 0; j < 480; j++)
    {
      if (i <= x0 + w && i > x0 && j < y0 + h && j >= y0)
      {
        DrawPoint(i, j, rgb);
      }
    }
  }
}

/*
 *@brief 画圆,圆形笔刷
 *@param x0,y0为圆心的位置,r为半径,rgb为自定义颜色,rgb是要写入屏幕的颜色
 *@return void
 *@author xiewanpi
 */
void drawyuan(int x0, int y0, int r, int rgb)
{

  for (int i = x0 - r; i < x0 + r; i++)
  {
    for (int j = y0 - r; j < y0 + r; j++)
    {
      if ((i - x0) * (i - x0) + (j - y0) * (j - y0) <= r * r)
      {
        DrawPoint(i, j, rgb);
      }
    }
  }
}

/*
 *@brief 清空屏幕
 */
void cleanscreen()
{
  for (int i = 0; i < 800; i++)
  {
    for (int j = 0; j < 480; j++)
    {
      DrawPoint(i, j, 0x000000);
    }
  }
}

/*
 *@brief 以圆形的方式展示图片,从中心开始
 */
void displaypicyuan(void *node)
{
  linknode *pic = node;
  int *rgb;
  int x0 = pic->w / 2;
  int y0 = pic->h / 2;
  int r = 80;
  printf("正在显示 %s\n", pic->data);
  if (!strcmp(pic->data + strlen(pic->data) - 4, ".bmp"))
  {
    while (r <= 700)
    {
      rgb = pic->color;
      for (int y = pic->h - 1; y >= 0; y--)
      {
        for (int x = 0; x < pic->w; x++)
        {
          if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r * r)
            DrawPoint(x, y, *rgb);
          rgb++;
        }
      }
      r += r_yuan;
    }
  }
  else
  {
    while (r <= 700)
    {
      rgb = pic->color + pic->h * pic->w - 1; // 与bmp存储的颜色相反
      for (int y = pic->h - 1; y >= 0; y--)
      {
        for (int x = 0; x < pic->w; x++)
        {
          if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r * r)
            DrawPoint(x, y, *rgb);
          rgb--;
        }
      }
      r += r_yuan;
    }
  }
}

/*
 *@brief 以圆形的方式展示图片,从边缘开始
 */
void displaypicyuanbian(void *node)
{
  linknode *pic = node;
  int *rgb;
  int x0 = pic->w / 2;
  int y0 = pic->h / 2;
  int r = 560;
  printf("正在显示 %s\n", pic->data);
  if (!strcmp(pic->data + strlen(pic->data) - 4, ".bmp"))
  {
    while (r >= 0)
    {
      rgb = pic->color;
      for (int y = pic->h - 1; y >= 0; y--)
      {
        for (int x = 0; x < pic->w; x++)
        {
          if ((x - x0) * (x - x0) + (y - y0) * (y - y0) < r * r)
            ;
          else
            DrawPoint(x, y, *rgb);
          rgb++;
        }
      }
      r -= r_yuan;
    }
  }
  else
  {
    while (r >= 0)
    {
      rgb = pic->color + pic->h * pic->w - 1; // 与bmp存储的颜色相反
      for (int y = pic->h - 1; y >= 0; y--)
      {
        for (int x = 0; x < pic->w; x++)
        {
          if ((x - x0) * (x - x0) + (y - y0) * (y - y0) < r * r)
            ;
          else
            DrawPoint(x, y, *rgb);
          rgb--;
        }
      }
      r -= r_yuan;
    }
  }
}

/*
 *@brief 自下而上渐变
 */
void displayjianbian(void *node)
{
  linknode *pic = node;
  int *rgb;
  int i = 12000;
  if (!strcmp(pic->data + strlen(pic->data) - 4, ".bmp"))
  {
    rgb = pic->color;
    for (int y = pic->h - 1; y >= 0; y--)
    {
      for (int x = 0; x < pic->w; x++)
      {
        DrawPoint(x, y, *rgb);
        rgb++;
      }
      delay_ms(200);
    }
  }
  else
  {
    rgb = pic->color + pic->h * pic->w - 1; // 与bmp存储的颜色相反
    for (int y = pic->h - 1; y >= 0; y--)
    {
      for (int x = 0; x < pic->w; x++)
      {
        DrawPoint(x, y, *rgb);
        rgb--;
      }
      delay_ms(200);
    }
  }
}

/*
 *@brief 自上而下渐变
 */
void displayjianbianxia(void *node)
{
  linknode *pic = node;
  int *rgb;
  if (!strcmp(pic->data + strlen(pic->data) - 4, ".bmp"))
  {
    rgb = pic->color + pic->h * pic->w - 1; // 移到图片开始的颜色
    for (int y = 0; y < pic->h - 1; y++)
    {
      for (int x = pic->w - 1; x >= 0; x--)
      {
        DrawPoint(x, y, *rgb);
        rgb--;
      }
      delay_ms(200);
    }
  }
  else
  {
    rgb = pic->color;
    for (int y = 0; y < pic->h - 1; y++)
    {
      for (int x = pic->w - 1; x >= 0; x--)
      {
        DrawPoint(x, y, *rgb);
        rgb++;
      }
      delay_ms(200);
    }
  }
}

/*
 *@brief 百叶窗
 */
void displaybaiye(void *node)
{
  linknode *pic = node;
  int *rgb;
  int line = 0;
  int len = 800 * 60;
  if (!strcmp(pic->data + strlen(pic->data) - 4, ".bmp"))
  {
    rgb = pic->color;
    for (int y = pic->h - 1; y >= 420; y--)
    {
      for (int x = 0; x < pic->w; x++)
      {
        for (int i = 0; i < 8; i++)
        {
          DrawPoint(x, y - i * 60, *(rgb + i * len));
        }
        rgb++;
      }
      delay_ms(1000);
    }
  }
  else
  {
    rgb = pic->color + pic->h * pic->w - 1; // 与bmp存储的颜色相反
    for (int y = pic->h - 1; y >= 420; y--)
    {
      for (int x = 0; x < pic->w; x++)
      {
        for (int i = 0; i < 8; i++)
        {
          DrawPoint(x, y - i * 60, *(rgb - i * len));
        }
        rgb--;
      }
      delay_ms(1000);
    }
  }
}

/*
 *@brief 划横线
 *@param x,y为横线起点坐标，x1为横线终止坐标
 */
void DrawLinehen(int x, int y, int x1)
{
  for (int i = x; i < x1; i++)
  {
    DrawPoint(i, y, 0x000000);
  }
}

/*
 *@brief 划竖线
 *@param x,y为竖线起点坐标，y1为竖线终止坐标
 */
void DrawLineshu(int x, int y, int y1)
{
  for (int i = y; i < y1; i++)
  {
    DrawPoint(x, i, 0x000000);
  }
}

/*
 *@brief 画线函数
 *@param x0,y0为竖线起点坐标，x1,y1为竖线终止坐标
 *@param bit,画笔大小
 *@param color,画笔颜色
 */
void DrawLine(int x0, int y0, int x1, int y1, int bit, int color)
{
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;
  static int brush = 0;
  for (;;)
  {
    drawyuan(x0, y0, bit, color);
    if (x0 == x1 && y0 == y1)
      break;
    e2 = 2 * err;
    if (e2 >= dy)
    {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx)
    {
      err += dx;
      y0 += sy;
    }
  }
}

/*
 *@brief 方形笔刷
 *@param x0,y0为矩形左上顶点的位置,r为方形的宽和高,rgb是要写入屏幕的颜色
 *@return void
 *@author xiewanpi
 */
void drawfaningbrush(int x0, int y0, int r, int rgb)
{
  for (int i = x0; i > x0 - r && i < x0 + r; i++)
  {
    for (int j = y0; j > y0 - r && j < y0 + r; j++)
    {
      DrawPoint(i, j, rgb);
    }
  }
}

/*
 *@brief 头发笔刷
 *@param x0,y0位置,r为半径,rgb为自定义颜色,rgb是要写入屏幕的颜色
 *@return void
 *@author xiewanpi
 */
void tofabrush(int x0, int y0, int r, int rgb)
{
  int to = 0;
  for (int i = x0 - r; i < x0 + r; i++)
  {
    for (int j = y0 - r; j < y0 + r; j++)
    {
      if ((i - x0) * (i - x0) + (j - y0) * (j - y0) <= r * r)
      {
        DrawPoint(i, j, rgb);
        to++;
      }
    }
  }
}

/*
 *@brief 喷枪笔刷
 *@param x0,y0位置,r为半径,rgb为自定义颜色,rgb是要写入屏幕的颜色
 *@return void
 *@author xiewanpi
 */
void penqian(int x0, int y0, int r, int rgb)
{
  int to = 0;
  for (int i = x0 - r; i < x0 + r; i++)
  {
    for (int j = y0 - r; j < y0 + r; j++)
    {
      if ((i - x0) * (i - x0) + (j - y0) * (j - y0) <= r * r)
      {
        DrawPoint(i, j, rgb);
        to++;
      }
    }
  }
}

/*
 *@brief 延时函数
 */
void delay_ms(int s)
{
  for (; s >= 0; s--)
    for (int y = 0; y < 1000; y++)
    {
      /* code */
    }
  return;
}