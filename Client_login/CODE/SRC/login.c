#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "Touch.h"
#include "fonts.h"
#include "BmpPicture.h"
#include "lcd.h"
#include "Client.h"
#include "login.h"
#include "interface.h"
#include "speech.h"
#include "Ledden.h"

unsigned char xing[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xC0, 0x01, 0xC0, 0x01, 0xC0, 0x30, 0xC6, 0x38, 0x8E, 0x1C, 0x9C, 0x06, 0xB0, 0x01, 0xC0,
    0x01, 0xC0, 0x06, 0xB0, 0x1C, 0x9C, 0x38, 0x8E, 0x31, 0x86, 0x01, 0xC0, 0x01, 0xC0, 0x01, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*"*",0*/ /* (16 X 32 , 宋体 )*/
};

int socktfd = -1;
/*
 *@brief 根据点击的坐标返回对应的数字
 *@param x 点击的x坐标
 *@param y 点击的y坐标
 *@return 对应的数字
 */
int keyboard_number(int x, int y)
{
  if (x > 280 && x < 350 && y > 240 && y < 290) // 1
  {
    return 1;
  }
  else if (x > 355 && x < 425 && y > 240 && y < 290) // 2
  {
    return 2;
  }
  else if (x > 435 && x < 505 && y > 240 && y < 290) // 3
  {
    return 3;
  }
  else if (x > 280 && x < 350 && y > 305 && y < 355) // 4
  {
    return 4;
  }
  else if (x > 355 && x < 425 && y > 305 && y < 355) // 5
  {
    return 5;
  }
  else if (x > 435 && x < 505 && y > 305 && y < 355) // 6
  {
    return 6;
  }
  else if (x > 280 && x < 350 && y > 365 && y < 415) // 7
  {
    return 7;
  }
  else if (x > 355 && x < 425 && y > 365 && y < 415) // 8
  {
    return 8;
  }
  else if (x > 435 && x < 505 && y > 365 && y < 415) // 9
  {
    return 9;
  }
  else if (x > 355 && x < 425 && y > 420 && y < 480) // 0
  {
    return 0;
  }
  return -1;
}

/*
 *@brief 注册界面

 */
char *Register(void)
{
  BmpPictureDisplay("/source/backgroundpic/register.bmp");
  int x, y;
  int x0 = 284, y0 = 64; // 定位防止出现坐标偏移
  int numx1 = 284;
  int numx2 = 284;
  int numx3 = 284;
  int numy1 = 64;
  int numy2 = 133;
  int numy3 = 188;
  int *numx = &numx1;
  int *numy = &numy1;

  char *zhanhao = malloc(14);
  char phone[14] = {0};
  char mima[14] = {0};
  char querenmima[14] = {0};
  while (1)
  {
    TouchDirection touch = GetTouchDirection(&x, &y);
    printf("x: %d y: %d\n", x, y);
    if (x && y) // 定位防止出现坐标偏移
    {
      x0 = x;
      y0 = y;
    }
    else if (0 == x)
    {
      x = x0;
    }
    else if (0 == y)
    {
      y == y0;
    }

    if (x > 280 && x < 505 && y > 240 && y < 480) // 输入数字
    {
      int num = keyboard_number(x, y); // 获得点击的数字
      if (-1 == num)
        continue;
      char chnum = num + '0';
      printf("num: %d\n", num);

      if (*numx < 444 || (numx == &numx1 && *numx <= 444))
      {
        *numx += 16;
        if (numx == &numx1) // 记录手机号码和密码
        {
          displaysingla(16, 32, *numx, *numy, num, 0x00, 0x9AA6B3);
          printf("chnum: %c\n", chnum);
          strncat(phone, &chnum, 1);
          printf("手机号码:%s\n", phone);
        }
        else if (numx == &numx2)
        {
          zimoDisplay(16, 32, *numx, *numy, xing, 0x00, 0x9AA6B3);
          strncat(mima, &chnum, 1);
          printf("密码:%s\n", mima);
        }
        else if (numx == &numx3)
        {
          zimoDisplay(16, 32, *numx, *numy, xing, 0x00, 0x9AA6B3);
          strncat(querenmima, &chnum, 1);
          printf("确认密码:%s\n", querenmima);
        }
      }
    }
    else if (x > 510 && x < 556 && y > 240 && y < 290) // 删除
    {
      if (*numx > 284)
      {
        drawjuxing(*numx, *numy, 16, 32, 0x9AA6B3);
        *numx -= 16;
        if (numx == &numx1) // 记录手机号码和密码
        {
          phone[strlen(phone) - 1] = '\0';
          printf("手机号码:%s\n", phone);
        }
        else if (numx == &numx2)
        {
          mima[strlen(mima) - 1] = '\0';
          printf("密码:%s\n", mima);
        }
        else if (numx == &numx3)
        {
          querenmima[strlen(querenmima) - 1] = '\0';
          printf("确认密码:%s\n", querenmima);
        }
      }
    }
    else if (x > 284 && x < 560 && y > 60 && y < 100) // 手机号码
    {
      numx = &numx1;
      numy = &numy1;
    }
    else if (x > 284 && x < 560 && y > 135 && y < 175) // 密码
    {
      numx = &numx2;
      numy = &numy2;
    }
    else if (x > 284 && x < 560 && y > 185 && y < 224) // 确认密码
    {
      printf("确认密码\n");
      numx = &numx3;
      numy = &numy3;
    }
    else if (x > 620 && x < 720 && y > 185 && y < 224) // 确认注册
    {
      if (11 != strlen(phone))
      {
        printf("手机号应该是11位数字,你输入了%d位,请重新输入\n", strlen(phone));
        numx1 = 284;
        numx = &numx1;
        numy = &numy1;
        drawjuxing(*numx, *numy, 178, 32, 0x9AA6B3);
        memset(phone, 0, strlen(phone));
        continue;
      }
      if (strcmp(mima, querenmima))
      {
        printf("两次输入的密码不一样,请重新输入\n");
        continue;
      }
      bag_data data;
      memset(&data, 0, sizeof(bag_data));
      sendbag("zuce", strlen(phone), phone), sizeof(bag_data);
      sendbag("zuce", strlen(mima), mima), sizeof(bag_data);
      printf("确认注册\n");
      RecvShujubao(&data, sizeof(bag_data));
      // printf("head:%s data:%s\n", data.head, data.data);
      char *account = malloc(14);
      memcpy(account, data.data, strlen(data.data));
      return account;
    }
    else if (x > 620 && x < 720 && y > 65 && y < 100) // 返回登陆
    {
      printf("返回\n");
      return NULL;
    }
  }
}

/*
 *@brief 登陆界面
 */
int Login()
{
  LcdInit();
  TouchInit();
  BmpPictureDisplay("/source/backgroundpic/login.bmp");
  int x, y;
  int x0 = 248, y0 = 64;
  int numx1 = 284;
  int numx2 = 284;
  int numy1 = 64;
  int numy2 = 133;
  int *numx = &numx1;
  int *numy = &numy1;
  char account[14] = {0};
  char password[14] = {0};
  socktfd = Client(); // 接收连接套接字
  if (-1 == socktfd)
  {
    printf("连接服务器失败\n");
  }
  while (1)
  {
    TouchDirection touch = GetTouchDirection(&x, &y);
    // printf("x: %d y: %d\n", x, y);

    printf("x: %d y: %d\n", x, y);
    if (x && y) // 定位防止出现坐标偏移
    {
      x0 = x;
      y0 = y;
    }
    else if (0 == x)
    {
      x = x0;
    }
    else if (0 == y)
    {
      y == y0;
    }

    if (x > 280 && x < 505 && y > 240 && y < 480) // 输入数字
    {
      int num = keyboard_number(x, y); // 获得点击的数字
      if (-1 == num)
        continue;
      printf("num: %d\n", num);
      char chnum = num + '0'; // 转换为字符
      if (*numx < 444)
      {
        *numx += 16;
        if (numx == &numx1) // 记录账号码和密码
        {
          displaysingla(16, 32, *numx, *numy, num, 0x00, 0x9AA6B3);
          printf("chnum: %c\n", chnum);
          strncat(account, &chnum, 1);
          printf("账号:%s\n", account);
        }
        else if (numx == &numx2)
        {
          zimoDisplay(16, 32, *numx, *numy, xing, 0x00, 0x9AA6B3);
          strncat(password, &chnum, 1);
          printf("密码:%s\n", password);
        }
      }
    }
    else if (x > 510 && x < 556 && y > 240 && y < 290) // 删除
    {
      if (*numx > 284)
      {
        drawjuxing(*numx, *numy, 16, 32, 0x9AA6B3);
        *numx -= 16;
        if (numx == &numx1) // 记录账号码和密码
        {
          account[strlen(account) - 1] = '\0';
          printf("账号:%s\n", account);
        }
        else if (numx == &numx2)
        {
          password[strlen(password) - 1] = '\0';
          printf("密码:%s\n", password);
        }
      }
    }
    else if (x > 284 && x < 560 && y > 60 && y < 100) // 账号
    {
      numx = &numx1;
      numy = &numy1;
    }
    else if (x > 284 && x < 560 && y > 135 && y < 175) // 密码
    {
      numx = &numx2;
      numy = &numy2;
    }
    else if (x > 268 && x < 365 && y > 188 && y < 224) // 登陆
    {
      bag_data data;
      memset(&data, 0, sizeof(bag_data));
      sendbag("zaho", strlen(account), account), sizeof(bag_data);
      sendbag("zaho", strlen(password), password), sizeof(bag_data);
      printf("登录\n");
      RecvShujubao(&data, sizeof(bag_data));
      printf("head:%s data:%s\n", data.head, data.data);
      if (!strncmp(data.head, "zaho", 4))
      {
        if (!strncmp(data.data, "ok", 2))
        {
          printf("登陆成功\n");
          interface(); // 登陆成功进入主界面
          printf("退出");
          BmpPictureDisplay("/source/backgroundpic/login.bmp");
          continue;
        }
        else if (!strncmp(data.data, "err", 3))
        {
          DispictoCenter("/source/backgroundpic/loginfail.bmp");
          printf("登陆失败，账号或密码错误\n");
          TouchDirection touch = GetTouchDirection(&x, &y); // 失败显示图片，点击后才可以继续登陆
        }
        BmpPictureDisplay("/source/backgroundpic/login.bmp");
        memset(account, 0, 14); // 进入的注册界面后，登陆界面的账号和密码需要重新输入
        memset(password, 0, 14);
        numx1 = 284;
        numx2 = 284;
        numx = &numx1;
        numy = &numy1;
      }
    }
    else if (x > 409 && x < 506 && y > 188 && y < 224) // 注册
    {
      memset(account, 0, 14); // 进入的注册界面后，登陆界面的账号和密码需要重新输入
      memset(password, 0, 14);
      numx1 = 284;
      numx2 = 284;
      numx = &numx1;
      numy = &numy1;
      printf("注册\n");
      char *zuceaccount = Register();
      BmpPictureDisplay("/source/backgroundpic/login.bmp");
      if (zuceaccount) // 如果返回了一个注册账号会自动填入
      {
        numx1 += 16;
        memcpy(account, zuceaccount, strlen(zuceaccount));
        printf("zuceaccount:%s\n", zuceaccount);
        int zhanhao = atoi(account);
        shuzizimodisplay(16, 32, numx1, numy1, zhanhao, 0x00, 0x9AA6B3);
        numx1 = 444;
        free(zuceaccount);
      }
      else
      {
        printf("取消注册\n");
      }
    }
    else if (Touchreturn == touch)
    {
      TouchDestroy();
      close_lcd();
      close(socktfd);
      break;
    }
  }
  return 0;
}

/*
 *@brief 开辟录音识别线程，录音4s，把录音文件发给客户端，服务器识别后发送识别到的指定id
 */
void *ResevSbid(void *arg)
{
  pthread_detach(pthread_self());
  bag_data data;
  memset(&data, 0, sizeof(bag_data));
  while (1)
  {
    printf("\t请录音4s!!!\n");
    speech_arecord();
    sendfile("hehe.pcm");
    RecvShujubao(&data, sizeof(bag_data));
    // printf("head:%s data:%s\n", data.head, data.data);
    if (!strncmp(data.head, "sbid", 4))
    {
      if (!strncmp(data.data, "no", 2))
      {
        printf("没有识别结果,继续识别\n");
        continue;
      }
      int id = atoi(data.data);
      printf("shibeid:%d\n", id);
      if (10001 != id) // 没有唤醒
      {
        printf("呼唤小鱼,唤醒智能管家\n");
        continue;
      }
      else // 唤醒小鱼
      {
        int flag = 0;
        while (1)
        {
          if (flag > 6)
          {
            printf("没有响应，小鱼休眠了\n");
            break;
          }
          memset(&data, 0, sizeof(bag_data));
          speech_arecord(); // 录入指令
          sendfile("hehe.pcm");
          RecvShujubao(&data, sizeof(bag_data));
          if (!strncmp(data.head, "sbid", 4))
          {
            if (!strncmp(data.data, "no", 2))
            {
              printf("没有识别结果,继续识别\n");
              flag++;
              continue;
            }
            flag = 0;
          }
          id = atoi(data.data);
          ContorSalsa(id);
        }
      }
    }
  }
}