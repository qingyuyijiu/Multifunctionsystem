#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "link.h"
#include "touch.h"
#include "BmpPicture.h"
#include "album.h"
#include "lcd.h"
#include "wuziqi.h"
#include "piviv.h"
#include "myplayer.h"
#include "EnvirDetection.h"
#include "pthread.h"
#include "Ledden.h"
#include "login.h"
#include "speech.h"

int *share1 = NULL;
int *share2 = NULL;
int *alsaid = NULL;
int flagmyplayer = 0;
int alsatui = -1; // alsatui>0 说明进入了某个软件， 所以只能识别退出

/*
 *@主页面
 *@param backgroundpathname 主页面背景
 */
int interface()
{
  int flag = 0;
  int tui = 0;
  int x, y;
  int hunjin = 0;
  TouchDirection touch; // 触摸事件
  int led_fd = led_init();
  printf("欢迎使用青鱼系统\n");
  pthread_t pthreadid; // 创建一个语音识别的线程
  int r = pthread_create(&pthreadid, NULL, ResevSbid, NULL);
  if (0 != r)
  {
    pthread_detach(pthreadid);
    printf("创建线程失败\n");
  }
  alsaid = malloc(sizeof(int));
  *alsaid = -1;
  printf("1/n");
  char buf[2] = {0, 8};
  key_t key1 = ftok("/home/xwp", 123); // 使用共享内存1，控制影音功能,-1退出
  if (-1 == key1)
  {
    printf("openftok error\n");
    return -1;
  }
  int shmid1 = shmget(key1, 4096, IPC_CREAT | 0666);
  if (-1 == shmid1)
  {
    printf("shmget error\n");
    return -1;
  }
  share1 = shmat(shmid1, NULL, 0);
  if (NULL == share1)
  {
    perror("shmat error");
    return -1;
  }

  key_t key2 = ftok("/home/xwp", 234); // 使用共享内存2，环境检测使用，-1退出
  if (-1 == key2)
  {
    printf("openftok error\n");
    return -1;
  }
  int shmid2 = shmget(key2, 4096, IPC_CREAT | 0666);
  if (-1 == shmid2)
  {
    printf("shmget error\n");
    return -1;
  }
  share2 = shmat(shmid2, NULL, 0);
  if (NULL == share2)
  {
    perror("shmat error");
    return -1;
  }

  drawjuxing(0, 448, 800, 32, 0xffffff); // 底线
  pthread_t pthread1;
  int pth1 = pthread_create(&pthread1, NULL, EnvirDetection, NULL); // 开辟采集环境数据的线程
  if (-1 == pth1)
  {
    perror("pth_create error");
    return -1;
  }

  // drawjuxing(0, 448, 800, 32, 0xffffff); // 底线
  while (1)
  {
    if (*alsaid < 0) // 如果通过语音控制入alsaid>1,就不能通过触摸进入了
    {
      printf("alsaid: %d\n", *alsaid);
      BmpPictureDisplay("/source/backgroundpic/background.bmp");
      if (1 == flagmyplayer)
      {
        flagmyplayer = 0;
        usleep(100000); // 可能还需要等待mplayer退出
      }
      BmpPictureDisplay("/source/backgroundpic/background.bmp");
      if (*share2 == -1) // 进入时这个线程是关闭的所以要重新打开
      {
        drawjuxing(0, 448, 800, 32, 0xffffff);                            // 底线
        int pth1 = pthread_create(&pthread1, NULL, EnvirDetection, NULL); // 开辟采集环境数据的线程
        if (-1 == pth1)
        {
          perror("pth_create error");
          return -1;
        }
      }
      if (-1 == *share2) // 说明进入了某一个功能
      {
        tui = 0;
        *share2 = 0;
      }
      touch = GetTouchDirection(&x, &y);
      if (*alsaid > 0)
      {
        printf("alsaid: %d\n", *alsaid);
        continue;
      }
      if (touch == Touchreturn)
      {
        if (1 == tui)
        {
          printf("退出青鱼\n");
          break;
        }
        else
          tui++;
      }
      else if (touch == TouchChick)
      {
        printf("点击 x:%d y:%d\n", x, y);
        if (x >= 50 && x < 135 && y >= 80 && y < 160) // 进入相册
        {
          *share2 = -1;
          DispictoCenter("/source/backgroundpic/jiazai.bmp");
          int r = album();
          if (-1 == r)
          {
            printf("相册打开失败\n");
            continue;
          }
          printf("退出相册\n");
        }
        if (x >= 275 && x < 355 && y >= 80 && y < 160) // 进入五子棋
        {
          *share2 = -1;
          DispictoCenter("/source/backgroundpic/jiazai.bmp");
          int r = wuziqi();
          if (-1 == r)
          {
            printf("五子棋打开失败\n");
            continue;
          }
          printf("退出五子棋\n");
        }
        if (x >= 480 && x < 560 && y >= 80 && y < 160) // 进入画板
        {
          *share2 = -1;
          DispictoCenter("/source/backgroundpic/jiazai.bmp");
          int r = piviv();
          if (-1 == r)
          {
            printf("画板打开失败\n");
            continue;
          }
          printf("退出画板\n");
        }
        if (x >= 675 && x < 760 && y >= 80 && y < 160) // 影音功能
        {
          *share2 = -1;
          usleep(500000);
          DispictoCenter("/source/backgroundpic/jiazai.bmp");
          int r = myplayer();
          while (1)
          {
            if (*share1 == -1)
              break;
          }
          if (-1 == r)
          {
            printf("影音功能打开失败\n");
            continue;
          }
          printf("退出影音功能\n");
          flagmyplayer = 1;
          continue;
        }
        if (x >= 675 && x < 760 && y >= 250 && y < 330) // 开关灯
        {
          if (1 == buf[0])
          {
            buf[0] = 0;
          }
          else
          {
            buf[0] = 1;
          }
          write(led_fd, buf, 2);
        }
      }
    }
  }
}

/*
 *@通过得到的id实现控制功能
 */
int ContorSalsa(int id)
{
  char buf[2] = {0, 8};
  int r = -1;
  switch (id)
  {
  case 12345:
    buf[0] = 1;
    int lcdk_fd = get_led_status();
    write(lcdk_fd, buf, 2);
    printf("\t\t\t开灯\n");
    break;
  case 54321:
    buf[0] = 0;
    int lcdg_fd = get_led_status();
    write(lcdg_fd, buf, 2);
    printf("\t\t\t关灯\n");
    break;
  case 23456:
    *alsaid = 0;
    printf("\t\t\t打开影视\n");
    *share2 = -1;
    usleep(500000);
    DispictoCenter("/source/backgroundpic/jiazai.bmp");
    r = myplayer();
    while (1)
    {
      if (*share1 == -1)
        break;
    }
    if (-1 == r)
    {
      printf("影音功能打开失败\n");
      break;
    }
    printf("退出影音功能\n");
    flagmyplayer = 1;
    *alsaid = -1;
    break;
  case 87654:
    printf("\t\t\t打开相册\n");
    *share2 = -1;
    *alsaid = 1;
    DispictoCenter("/source/backgroundpic/jiazai.bmp");
    r = album();
    if (-1 == r)
    {
      printf("相册打开失败\n");
      break;
    }
    printf("退出相册\n");
    *alsaid = -1;
    break;
  case 98765:
    printf("\t\t\t打开五子棋\n");
    *share2 = -1;
    *alsaid = 2;
    DispictoCenter("/source/backgroundpic/jiazai.bmp");
    r = wuziqi();
    if (-1 == r)
    {
      printf("五子棋打开失败\n");
      break;
    }
    printf("退出五子棋\n");
    *alsaid = -1;
    break;
  case 45678:
    printf("\t\t\t打开画板\n");
    *alsaid = 3;
    *share2 = -1;
    DispictoCenter("/source/backgroundpic/jiazai.bmp");
    r = piviv();
    if (-1 == r)
    {
      printf("画板打开失败\n");
      break;
    }
    printf("退出画板\n");
    *alsaid = -1;
    break;
  default:
    printf("\t\t\t你说什么，没有听清\n");
    break;
  }
  return 0;
}