#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <lcd.h>
#include <unistd.h>
#include "fonts.h"

unsigned char yi[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x80,
    0x1F, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
    0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
    0x01, 0x80, 0x01, 0x80, 0x03, 0xC0, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"1",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};

unsigned char er[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x08, 0x38,
    0x10, 0x18, 0x20, 0x0C, 0x20, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x00, 0x0C, 0x00, 0x18, 0x00, 0x18,
    0x00, 0x30, 0x00, 0x60, 0x00, 0xC0, 0x01, 0x80, 0x03, 0x00, 0x02, 0x00, 0x04, 0x04, 0x08, 0x04,
    0x10, 0x04, 0x20, 0x0C, 0x3F, 0xF8, 0x3F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"2",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};
unsigned char sang[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x18, 0x60,
    0x30, 0x30, 0x30, 0x18, 0x30, 0x18, 0x30, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x30, 0x00, 0x60,
    0x03, 0xC0, 0x00, 0x70, 0x00, 0x18, 0x00, 0x08, 0x00, 0x0C, 0x00, 0x0C, 0x30, 0x0C, 0x30, 0x0C,
    0x30, 0x08, 0x30, 0x18, 0x18, 0x30, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"3",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};
unsigned char si[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x60,
    0x00, 0xE0, 0x00, 0xE0, 0x01, 0x60, 0x01, 0x60, 0x02, 0x60, 0x04, 0x60, 0x04, 0x60, 0x08, 0x60,
    0x08, 0x60, 0x10, 0x60, 0x30, 0x60, 0x20, 0x60, 0x40, 0x60, 0x7F, 0xFC, 0x00, 0x60, 0x00, 0x60,
    0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x03, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"4",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};
unsigned char wu[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFC, 0x0F, 0xFC,
    0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x13, 0xE0, 0x14, 0x30,
    0x18, 0x18, 0x10, 0x08, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x30, 0x0C, 0x30, 0x0C,
    0x20, 0x18, 0x20, 0x18, 0x18, 0x30, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"5",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};
unsigned char liu[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x06, 0x18,
    0x0C, 0x18, 0x08, 0x18, 0x18, 0x00, 0x10, 0x00, 0x10, 0x00, 0x30, 0x00, 0x33, 0xE0, 0x36, 0x30,
    0x38, 0x18, 0x38, 0x08, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x18, 0x0C,
    0x18, 0x08, 0x0C, 0x18, 0x0E, 0x30, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"6",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};
unsigned char qi[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFC, 0x1F, 0xFC,
    0x10, 0x08, 0x30, 0x10, 0x20, 0x10, 0x20, 0x20, 0x00, 0x20, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x80, 0x00, 0x80, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00,
    0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"7",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};
unsigned char ba[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x0C, 0x30,
    0x18, 0x18, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x38, 0x0C, 0x38, 0x08, 0x1E, 0x18, 0x0F, 0x20,
    0x07, 0xC0, 0x18, 0xF0, 0x30, 0x78, 0x30, 0x38, 0x60, 0x1C, 0x60, 0x0C, 0x60, 0x0C, 0x60, 0x0C,
    0x60, 0x0C, 0x30, 0x18, 0x18, 0x30, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"8",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};
unsigned char jiu[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x18, 0x20,
    0x30, 0x10, 0x30, 0x18, 0x60, 0x08, 0x60, 0x0C, 0x60, 0x0C, 0x60, 0x0C, 0x60, 0x0C, 0x60, 0x0C,
    0x70, 0x1C, 0x30, 0x2C, 0x18, 0x6C, 0x0F, 0x8C, 0x00, 0x0C, 0x00, 0x18, 0x00, 0x18, 0x00, 0x10,
    0x30, 0x30, 0x30, 0x60, 0x30, 0xC0, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"9",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};

unsigned char ling[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x06, 0x20,
    0x0C, 0x30, 0x18, 0x18, 0x18, 0x18, 0x18, 0x08, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C,
    0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x18, 0x08, 0x18, 0x18,
    0x18, 0x18, 0x0C, 0x30, 0x06, 0x20, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"0",0*/
};
unsigned char diang[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*".",0*/
                                                                                                    /* (16 X 32 , 宋体 )*/
};

/*
 *@brief 显示字模，顺式，高位在前，低位在后
 *@param w 字模宽度
 *@param h 字模高度
 *@param x0 字模显示起始x坐标
 *@param y0 字模显示起始y坐标
 *@param zimo 字模数据
 *@param zicolor 字颜色
 *@param dicolor 底颜色
 */
void zimoDisplay(int w, int h, int x0, int y0, unsigned char *zimo, unsigned int zicolor, unsigned int dicolor) // 顺序
{
  int i = 0; // 按字节循环数据
  int x, y;
  int kuandu = w % 8 ? w / 8 + 1 : w / 8;
  int n = (8 - w % 8) % 8; // 后面补了n个字节
  for (i = 0; i < kuandu * h; i++)
  {
    // x y坐标
    y = i / kuandu; // kuandu一行有多少个字节
    // zimo[i] -> 就是循环它的字节的
    for (int j = 7; j >= 0; j--) // 将这个字节分解为bit  顺向  高bit在前
    {
      x = i % kuandu * 8 + (7 - j);
      if (i % kuandu == kuandu - 1 && w % 8) // 补了几个bit n = (8 - w % 8) % 8;
      {
        if (j < n) // 将补的n个bit跳过
        {
          break;
        }
      }
      // 阴码是1亮  所以这个bit为1的地方的点亮
      if (zimo[i] & 1 << j)
      {
        DrawPoint(x + x0, y + y0, zicolor); // 红字
      }
      else
      {
        DrawPoint(x + x0, y + y0, dicolor); // 白底
      }
    }
  }
}

/*
 *@brief 显示数字模，顺式，高位在前，低位在后
 *@param w 字模宽度
 *@param h 字模高度
 *@param x0 字模显示起始x坐标
 *@param y0 字模显示起始y坐标
 *@param num 要显示的数字
 *@param zicolor 字颜色
 *@param dicolor 底颜色
 *@note 目前只能显示小数，整数最大只能显示4位
 *@author xwp
 */
int shuzizimodisplay(int w, int h, int x0, int y0, int num, unsigned int zicolor, unsigned int dicolor)
{
  int singalnum = 0; // 按字节循环数据
  int weishu = 1;
  int i = 1;
  char *suzi;
  while (num / weishu > 9) // 判断num有几位数
  {
    weishu *= 10;
    i++;
  }

  for (; i > 0; i--)
  {
    singalnum = num / weishu; // 取出第i位的数字
    switch (singalnum)
    {
    case 0:
      suzi = ling;
      break;
    case 1:
      suzi = yi;
      break;
    case 2:
      suzi = er;
      break;
    case 3:
      suzi = sang;
      break;
    case 4:
      suzi = si;
      break;
    case 5:
      suzi = wu;
      break;
    case 6:
      suzi = liu;
      break;
    case 7:
      suzi = qi;
      break;
    case 8:
      suzi = ba;
      break;
    case 9:
      suzi = jiu;
      break;
    default:
      break;
    }
    zimoDisplay(w, h, x0, y0, suzi, zicolor, dicolor); // 显示最高位数字
    num %= weishu;                                     // 去掉最高位的数字
    weishu /= 10;
    x0 += 16; // 右移16个像素显示下一个数字
  }
}

/*
 *@brief 显示一个数字模，顺式，高位在前，低位在后
 *@param w 字模宽度
 *@param h 字模高度
 *@param x0 字模显示起始x坐标
 *@param y0 字模显示起始y坐标
 *@param num 要显示的数字
 *@param zicolor 字颜色
 *@param dicolor 底颜色
 *@note 目前只能显示小数，整数最大只能显示4位
 *@author xwp
 */
int displaysingla(int w, int h, int x0, int y0, int num, unsigned int zicolor, unsigned int dicolor)
{
  char *suzi = NULL;
  switch (num)
  {
  case 0:
    suzi = ling;
    break;
  case 1:
    suzi = yi;
    break;
  case 2:
    suzi = er;
    break;
  case 3:
    suzi = sang;
    break;
  case 4:
    suzi = si;
    break;
  case 5:
    suzi = wu;
    break;
  case 6:
    suzi = liu;
    break;
  case 7:
    suzi = qi;
    break;
  case 8:
    suzi = ba;
    break;
  case 9:
    suzi = jiu;
    break;
  default:
    return 0;
  }
  zimoDisplay(w, h, x0, y0, suzi, zicolor, dicolor); // 显示一个数字
  usleep(100000);
  return 0;
}