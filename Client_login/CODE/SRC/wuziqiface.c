#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BmpPicture.h"
#include "wuziqiface.h"
#include "lcd.h"

/*
 *@brief 显示五子棋界面
 *@param pathname 五子棋界面图片的路径
 */
int DisplayQiban(const char *pathname)
{
  BmpPictureDisplay(pathname);
  for (int i = 15; i <= 690; i += 45) // 画棋盘横线
  {
    DrawLinehen(15, i, 690);
  }
  for (int i = 15; i <= 690; i += 45) // 画棋盘竖线
  {
    DrawLineshu(i, 15, 465);
  }
  return 0;
}
