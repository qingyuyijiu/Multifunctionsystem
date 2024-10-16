#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "wuziqi.h"
#include "wuziqiface.h"
#include "touch.h"
#include "lcd.h"
#include "BmpPicture.h"

#define WIN_NUM 5                  // 五子棋
int win = -1;                      // 没有获胜者为-1,获胜方黑为0，白为1
int qiziArry[14][9] = {-1};        // 存储棋子是否已经下了，没下为-1,黑为0，白为1 一行有14个，一列有9个棋子,下标与触摸点坐标[x / 45 - 1][y / 45 - 1]对应
#define Qizix(j) (i + 1) * 45 + 15 // 通过第二个下标计算棋子圆心所在的x位置
#define Qiziy(i) (j + 1) * 45 + 15 // 通过第一个下标计算棋子圆心所在的y位置
int qizi = 0;                      // 0表示为下黑，五子棋黑子先行

/*
 *@brief 实现五子棋功能
 */
int wuziqi()
{
  int x, y;     // 存储按下的坐标，实现下子功能
  int r;        // 接收异常
  cleanqipan(); // 每次进入时需要清空棋子数据
  printf("进入五子棋游戏\n");
  DisplayQiban("/source/backgroundpic/qiban.bmp");
  while (1)
  {
    TouchDirection touch = GetTouchDirection(&x, &y);
    printf("x:%d y:%d\n", x, y);
    if (touch == Touchreturn) // 返回界面
    {
      break;
    }
    if (x > 730 && x < 800 && y > 400 && y < 425) // 点击此处游戏重新开始
    {
      DisplayQiban("/source/backgroundpic/qiban.bmp");
      cleanqipan();
    }
    if (win == 0 || win == 1) // 如果有获胜者了，就不必继续一下操作了
      continue;
    if (x / 45 - 1 < 0 || y / 45 - 1 < 0 || x / 45 - 1 > 13 || y / 45 - 1 > 8) // 越界了，需要重新下
    {
      printf("请再下一次");
      continue;
    }
    if (-1 == DrawQizi(qizi, x, y)) // 画棋子,等于-1就不画
    {
      printf("此处已有棋子\n");
      continue; // 结束此次循环
    }
    if (0 == win)
    {
      DispictoCenter("/source/backgroundpic/heiqisheng.bmp");
      printf("黑子获胜\n");
    }
    else if (1 == win)
    {
      DispictoCenter("/source/backgroundpic/baiqisheng.bmp");
      printf("白子获胜\n");
    }
    qizi = qizi ^ 1; // 异或取反，黑白交替
  }
  return 0;
}

/*
 *@brief 实现画棋子的功能
 *@return 返回-1表示此处已经有棋子了
 *@return 0 表示正常可以下
 */
int DrawQizi(int qizi, int x, int y)
{
  printf("x:%d y:%d\n", x, y);
  printf("棋子坐标(%d,%d)\n", x / 45 - 1, y / 45 - 1);
  int rgb = 0;
  int i = x / 45 - 1; // 坐标转换为对应下标
  int j = y / 45 - 1;
  int x0 = Qizix(j); // 棋子圆心坐标
  int y0 = Qiziy(i);
  if (-1 == Justqizi(i, j)) // 没有被下
  {
    if (qizi) // 如果棋子为1 就是要下白棋。颜色为白色
      rgb = 0xffffff;
    else // 0x00表示黑色
      rgb = 0x00;
    drawyuan(x0, y0, 22, rgb);
    qiziArry[i][j] = qizi; // 记录此处的棋子的颜色
    win = wingame(i, j);
  }
  else // 此处棋子被下了
  {
    printf("\n");
    return -1;
  }
  printf("\n");
  return 0;
}

/*
 *@brief 判断要下的位置上面是否已经有棋子
 */
int Justqizi(int i, int j)
{
  return qiziArry[i][j];
}

/*
 *@brief 判断游戏是否结束
 *@param i,j棋子下标
 *@return -1表示游戏还未结束，0表示黑子胜出，1表示白子胜出
 *@author xiewanpi
 */
int wingame(int i, int j)
{
  int n = qiziArry[i][j];
  if (-1 == n) // 表示此处没有棋子,返回异常
  {
    return -1;
  }

  int count = 0; // 记录连续棋子的个数

  printf("颜色:%d\n", n);
  // 同行棋子
  for (int x = i; x >= 0 && qiziArry[x][j] == n; x--) // 前面有没有颜色相同的
    count++;
  printf("前：%d\n", count);
  for (int x = i + 1; x < 14 && qiziArry[x][j] == n; x++) // 后面有没有颜色相同的
    count++;
  printf("后：%d\n", count);
  if (count >= WIN_NUM)
    return n;

  count = 0; // 进入下一轮判断count需要重置
  // 同列棋子
  for (int y = j; y >= 0 && qiziArry[i][y] == n; y--) // 上面有没有颜色相同的
    count++;
  for (int y = j + 1; y < 9 && qiziArry[i][y] == n; y++) // 下面有没有颜色相同的
    count++;
  if (count >= WIN_NUM)
    return n;

  count = 0; // 进入下一轮判断count需要重置
  // 45度棋子
  for (int x = i, y = j; x < 14 && y >= 0 && qiziArry[x][y] == n; x++, y--) // 右上有没有颜色相同的
    count++;
  for (int x = i - 1, y = j + 1; x >= 0 && y < 9 && qiziArry[x][y] == n; x--, y++) // 左下有没有颜色相同的
    count++;
  if (count >= WIN_NUM)
    return n;

  count = 0; // 进入下一轮判断count需要重置
  // 135度棋子
  for (int x = i, y = j; x >= 0 && y >= 0 && qiziArry[x][y] == n; x--, y--) // 左上有没有颜色相同的
    count++;
  for (int x = i + 1, y = j + 1; x < 14 && y < 9 && qiziArry[x][y] == n; x++, y++) // 右下有没有颜色相同的
    count++;
  if (count >= WIN_NUM)
    return n;

  return -1; // 游戏还没有结束返回-1
}

/*
 *@brief 清空棋盘
 */
void cleanqipan()
{
  win = -1; // 获胜者重置
  qizi = 0;
  for (int i = 0; i < 14; i++) // 初始化棋子数组
    for (int j = 0; j < 9; j++)
    {
      qiziArry[i][j] = -1;
    }
}