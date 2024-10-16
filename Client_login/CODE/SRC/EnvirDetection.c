#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include "EnvirDetection.h"
#include "lcd.h"
#include "serial_init.h"
#include "fonts.h"
#include "Ledden.h"

unsigned char guan[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x00,
    0x00, 0x01, 0x80, 0x00, 0x02, 0x01, 0x80, 0x00, 0x01, 0x81, 0x81, 0x80, 0x00, 0xC1, 0x83, 0x80,
    0x00, 0x61, 0x83, 0x00, 0x00, 0x71, 0x86, 0x00, 0x00, 0x31, 0x86, 0x00, 0x00, 0x31, 0x84, 0x00,
    0x00, 0x01, 0x88, 0x10, 0x00, 0x01, 0x90, 0x30, 0x3F, 0xFF, 0xFF, 0xF8, 0x00, 0x18, 0x30, 0x00,
    0x00, 0x18, 0x30, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x18, 0x30, 0x00,
    0x00, 0x10, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x08, 0x00, 0x20, 0x30, 0x08,
    0x00, 0x60, 0x30, 0x08, 0x00, 0x40, 0x30, 0x08, 0x00, 0xC0, 0x30, 0x08, 0x01, 0x00, 0x30, 0x0C,
    0x06, 0x00, 0x3F, 0xFC, 0x18, 0x00, 0x1F, 0xF8, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*"光",0*/ /* (32 X 32 , 宋体 )*/
};

unsigned char zhao[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x13, 0xFF, 0xF0,
    0x0F, 0xF8, 0x30, 0x30, 0x0C, 0x30, 0x30, 0x20, 0x0C, 0x30, 0x30, 0x60, 0x0C, 0x30, 0x20, 0x60,
    0x0C, 0x30, 0x60, 0x60, 0x0C, 0x30, 0xC7, 0xC0, 0x0C, 0x30, 0x81, 0xC0, 0x0F, 0xF1, 0x01, 0x80,
    0x0C, 0x32, 0x80, 0x20, 0x0C, 0x34, 0xFF, 0xF0, 0x0C, 0x30, 0xC0, 0x60, 0x0C, 0x30, 0xC0, 0x60,
    0x0C, 0x30, 0xC0, 0x60, 0x0C, 0x30, 0xC0, 0x60, 0x0C, 0x30, 0xC0, 0x60, 0x0F, 0xF0, 0xFF, 0xE0,
    0x0C, 0x30, 0xC0, 0x60, 0x0C, 0x30, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0x40, 0x40,
    0x04, 0x10, 0x30, 0x60, 0x04, 0x18, 0x38, 0x30, 0x0C, 0x0C, 0x18, 0x38, 0x1C, 0x0C, 0x18, 0x18,
    0x1C, 0x0C, 0x18, 0x18, 0x18, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*"照",0*/ /* (32 X 32 , 宋体 )*/
};

unsigned char wen[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x08, 0x00, 0x80,
    0x06, 0x0F, 0xFF, 0xC0, 0x03, 0x0C, 0x00, 0x80, 0x03, 0x0C, 0x00, 0x80, 0x00, 0x2C, 0x00, 0x80,
    0x00, 0x4C, 0x00, 0x80, 0x00, 0x4F, 0xFF, 0x80, 0x00, 0x4C, 0x00, 0x80, 0x30, 0x4C, 0x00, 0x80,
    0x18, 0x8C, 0x00, 0x80, 0x0C, 0x8C, 0x00, 0x80, 0x08, 0x8F, 0xFF, 0x80, 0x01, 0x0C, 0x00, 0x80,
    0x01, 0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x20, 0x00, 0x20, 0x02, 0x3F, 0xFF, 0xF0,
    0x02, 0x31, 0x8C, 0x60, 0x06, 0x31, 0x8C, 0x60, 0x3E, 0x31, 0x8C, 0x60, 0x0E, 0x31, 0x8C, 0x60,
    0x0C, 0x31, 0x8C, 0x60, 0x0C, 0x31, 0x8C, 0x60, 0x0C, 0x31, 0x8C, 0x60, 0x0C, 0x31, 0x8C, 0x60,
    0x0C, 0x31, 0x8C, 0x6C, 0x0D, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"温",0*/
                                                                                                    /* (32 X 32 , 宋体 )*/
};

unsigned char du[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00,
    0x00, 0x00, 0xE0, 0x10, 0x04, 0x00, 0x40, 0x38, 0x07, 0xFF, 0xFF, 0xFC, 0x06, 0x04, 0x04, 0x00,
    0x06, 0x07, 0x07, 0x00, 0x06, 0x06, 0x06, 0x00, 0x06, 0x06, 0x06, 0x30, 0x07, 0xFF, 0xFF, 0xF8,
    0x06, 0x06, 0x06, 0x00, 0x06, 0x06, 0x06, 0x00, 0x06, 0x06, 0x06, 0x00, 0x06, 0x06, 0x06, 0x00,
    0x06, 0x07, 0xFE, 0x00, 0x06, 0x06, 0x06, 0x00, 0x04, 0x00, 0x01, 0x00, 0x04, 0x3F, 0xFF, 0x80,
    0x0C, 0x04, 0x03, 0x80, 0x0C, 0x02, 0x07, 0x00, 0x0C, 0x03, 0x06, 0x00, 0x08, 0x01, 0x0C, 0x00,
    0x08, 0x00, 0xD8, 0x00, 0x18, 0x00, 0x70, 0x00, 0x10, 0x00, 0xF8, 0x00, 0x10, 0x03, 0x9E, 0x00,
    0x20, 0x0E, 0x07, 0xF8, 0x20, 0x70, 0x00, 0xF8, 0x47, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, /*"度",0*/
                                                                                                    /* (32 X 32 , 宋体 )*/
};

unsigned char shi[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00,
    0x06, 0x0F, 0xFF, 0xE0, 0x07, 0x0C, 0x00, 0x60, 0x03, 0x0C, 0x00, 0x60, 0x03, 0x0C, 0x00, 0x60,
    0x00, 0x2C, 0x00, 0x60, 0x00, 0x4F, 0xFF, 0xE0, 0x60, 0x4C, 0x00, 0x60, 0x30, 0x4C, 0x00, 0x60,
    0x1C, 0x8C, 0x00, 0x60, 0x0C, 0x8C, 0x00, 0x60, 0x0C, 0x8F, 0xFF, 0xE0, 0x01, 0x0C, 0x84, 0x60,
    0x01, 0x08, 0xC6, 0x00, 0x03, 0x00, 0xC6, 0x00, 0x02, 0x00, 0xC6, 0x18, 0x06, 0x20, 0xC6, 0x1C,
    0x06, 0x10, 0xC6, 0x30, 0x7C, 0x18, 0xC6, 0x60, 0x0C, 0x0C, 0xC6, 0x40, 0x0C, 0x0C, 0xC6, 0x80,
    0x0C, 0x0C, 0xC7, 0x00, 0x0C, 0x04, 0xC6, 0x00, 0x1C, 0x00, 0xC6, 0x00, 0x1C, 0x00, 0xC6, 0x00,
    0x1C, 0x00, 0xC6, 0x08, 0x0C, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"湿",0*/
                                                                                                    /* (32 X 32 , 宋体 )*/
};

unsigned char yan[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x84, 0x00, 0x08,
    0x01, 0x07, 0xFF, 0xFC, 0x01, 0x06, 0x08, 0x18, 0x01, 0x06, 0x0C, 0x18, 0x01, 0x16, 0x0C, 0x18,
    0x01, 0x1E, 0x0C, 0x18, 0x01, 0x36, 0x0C, 0x18, 0x01, 0x26, 0x0C, 0x18, 0x11, 0x46, 0x0C, 0x98,
    0x11, 0x86, 0xFF, 0xF8, 0x19, 0x06, 0x0C, 0x18, 0x19, 0x06, 0x0C, 0x18, 0x31, 0x06, 0x0C, 0x18,
    0x21, 0x06, 0x0C, 0x18, 0x01, 0x06, 0x0C, 0x18, 0x01, 0x06, 0x0A, 0x18, 0x03, 0x06, 0x09, 0x18,
    0x03, 0x06, 0x19, 0x98, 0x03, 0xC6, 0x10, 0xD8, 0x02, 0x66, 0x30, 0xD8, 0x02, 0x36, 0x20, 0xD8,
    0x06, 0x1E, 0x40, 0x18, 0x04, 0x1E, 0x80, 0x18, 0x0C, 0x06, 0x00, 0x18, 0x08, 0x07, 0xFF, 0xF8,
    0x10, 0x06, 0x00, 0x18, 0x20, 0x06, 0x00, 0x18, 0x40, 0x06, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, /*"烟",0*/
                                                                                                    /* (32 X 32 , 宋体 )*/
};

unsigned char yanwudwu[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0xFF, 0xFF, 0x80,
    0x00, 0x01, 0x80, 0x00, 0x04, 0x01, 0x80, 0x10, 0x07, 0xFF, 0xFF, 0xF8, 0x04, 0x01, 0x80, 0x30,
    0x0C, 0xF9, 0x9F, 0x20, 0x1C, 0x01, 0x80, 0x40, 0x00, 0xF9, 0x9F, 0x00, 0x00, 0x01, 0x80, 0x00,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0xD0, 0x0E, 0x00,
    0x01, 0x88, 0x18, 0x00, 0x02, 0x06, 0x60, 0x00, 0x0C, 0x01, 0xC0, 0x00, 0x00, 0x0E, 0x7C, 0x00,
    0x00, 0x72, 0x0F, 0xF8, 0x0F, 0x83, 0x00, 0xF0, 0x30, 0x03, 0x01, 0x00, 0x01, 0xFF, 0xFF, 0x80,
    0x00, 0x06, 0x03, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x0C, 0x03, 0x00, 0x00, 0x18, 0x02, 0x00,
    0x00, 0x30, 0x46, 0x00, 0x01, 0xC0, 0x3C, 0x00, 0x1E, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, /*"雾",1*/
                                                                                                    /* (32 X 32 , 宋体 )*/
};

unsigned char mohao[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0xC0, 0x03, 0xC0,
    0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x80, 0x03, 0xC0, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*":"16x32,0*/
};

#define ZICOLOR 0xff00
#define DICOLOR 0xffffff
/*
 *@brief 初始化pwm
 */
int pwm_init()
{
  int fd = open("/dev/pwm", O_RDWR);
  if (-1 == fd)
  {
    perror("open led_drv error");
    return -1;
  }
  return fd;
}

/*
 *@brief 采集光照，温度，湿度。并显示在屏幕上
 *@note 还可以采集海拔高度和压强，这里没有给出
 */
void *GY_39(void *arg)
{
  int *fanhui = shaerneric();
  // 初始化接GY39的那个串口
  printf("GY39 thread start\n");
  int fd = init_serial("/dev/ttySAC1", 9600);
  if (-1 == fd)
  {
    return NULL;
  }
  int led_fd = get_led_status();
  char buf[2];
  unsigned char cmd[3] = {0xa5, 0x83, 0x28};
  unsigned char recvdata[24] = {0x01, 0x02, 0x03};
  int flag = 0;
  write(fd, cmd, 3); // GY39模块只需要发送一次命令就可以了
  int i = 0;
  while (1)
  {
    if (*fanhui < 0)
      break;
    int r = read(fd, &recvdata[flag++], 1);
    if (r <= 0)
    {
      if (r == 0)
      {
        flag--;
        continue;
      }
      else if (r == -1)
      {

        perror("read serial error");
        return NULL;
      }
    }
    switch (flag)
    {
    case 1:
      if (recvdata[0] != 0x5a)
      {
        flag = 0;
      }
      continue;
    case 2:
      if (recvdata[1] != 0x5a)
      {
        flag = 0;
      }
      continue;
    case 3:
      if (recvdata[2] != 0x15)
      {
        flag = 0;
      }
      continue;
    case 4:
      if (recvdata[3] != 0x04)
      {
        flag = 0;
      }
      continue;
    case 24:
      flag = 0;
      int lux = (recvdata[4] << 24 | recvdata[5] << 16 | recvdata[6] << 8 | recvdata[7]) / 100;
      int temp = (recvdata[13] << 8 | recvdata[14]) / 100;
      int Hum = (recvdata[19] << 8 | recvdata[20]) / 100;
      // printf("lux = %d    temp = %d  Hum = %d\n", lux, temp, Hum);
      if (lux < 20)
      {
        buf[0] = 1; // 亮灯
        buf[1] = 9; // D9
        write(led_fd, buf, 2);
      }
      else
      {
        buf[0] = 0; // 灭灯
        buf[1] = 9; // D9
        write(led_fd, buf, 2);
      }
      shuzizimodisplay(16, 32, 80, 448, lux, ZICOLOR, DICOLOR);   // 显示光照
      shuzizimodisplay(16, 32, 224, 448, temp, ZICOLOR, DICOLOR); // 显示温度
      shuzizimodisplay(16, 32, 368, 448, Hum, ZICOLOR, DICOLOR);  // 显示湿度
      sleep(1);
    default:
      continue;
    }
  }
  printf("GY39 exit\n");
  close(fd);
}

/*
 *@brief 采集烟雾浓度，并显示在屏幕上
 */
void *MQ2(void *arg)
{
  printf("MQ2 thread start\n");
  int *flag = shaerneric();
  // 初始化接GY39的那个串口
  int MQ2_fd = init_serial("/dev/ttySAC2", 9600);
  if (-1 == MQ2_fd)
  {
    return NULL;
  }
  int pwm_fd = pwm_init();
  char buf[1];
  unsigned char cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  unsigned char recvdata[9] = {0x01, 0x02, 0x03};
  int i = 0;
  int r;
  while (1)
  {
    if (*flag < 0)
      break;
    sleep(1);
    write(MQ2_fd, cmd, 9);
  hehe:
    if (*flag < 0)
      break;
    r = read(MQ2_fd, recvdata, 9);
    if (-1 == r)
    {
      perror("read error");
      return NULL;
    }
    else if (r == 0)
    {
      goto hehe;
    }
    int yanwu = recvdata[2] << 8 | recvdata[3];
    printf("yanwu = %d\n", yanwu);
    if (yanwu < 244)
    {
      drawjuxing(512, 448, 48, 32, 0xffffff);
      shuzizimodisplay(16, 32, 512, 448, yanwu, ZICOLOR, DICOLOR); // 显示烟雾浓度
      if (yanwu > 150)
      {
        buf[0] = 1; // 响
        write(pwm_fd, buf, 1);
      }
      else
      {
        buf[0] = 0; // 不响
        write(pwm_fd, buf, 1);
      }
      int gewei = yanwu % 10;
      int shiwei = yanwu / 10 % 10;
      int baiwei = yanwu / 100 % 10;
    }
  }
  printf("MQ2 exit\n");
  close(MQ2_fd);
  close(pwm_fd);
}

/*
 *@brief 显示环境数据，包括关照，温度，湿度和烟雾浓度
 *@note 使用了两个线程，同时使用GY39和MQ2设备开始采集
 *@author xwp
 */
void *EnvirDetection(void *arg)
{
  pthread_detach(pthread_self());
  zimoDisplay(32, 32, 0, 448, guan, ZICOLOR, DICOLOR);
  zimoDisplay(32, 32, 32, 448, zhao, ZICOLOR, DICOLOR);
  zimoDisplay(16, 32, 64, 448, mohao, ZICOLOR, DICOLOR);

  zimoDisplay(32, 32, 144, 448, wen, ZICOLOR, DICOLOR);
  zimoDisplay(32, 32, 176, 448, du, ZICOLOR, DICOLOR);
  zimoDisplay(16, 32, 208, 448, mohao, ZICOLOR, DICOLOR);

  zimoDisplay(32, 32, 288, 448, shi, ZICOLOR, DICOLOR);
  zimoDisplay(32, 32, 320, 448, du, ZICOLOR, DICOLOR);
  zimoDisplay(16, 32, 352, 448, mohao, ZICOLOR, DICOLOR);

  zimoDisplay(32, 32, 432, 448, yan, ZICOLOR, DICOLOR);
  zimoDisplay(32, 32, 464, 448, yanwudwu, ZICOLOR, DICOLOR);
  zimoDisplay(16, 32, 496, 448, mohao, ZICOLOR, DICOLOR);

  // 打开驱动
  int *flag = shaerneric();
  pthread_t thread1;
  int pth1 = pthread_create(&thread1, NULL, GY_39, NULL);
  if (-1 == pth1)
  {
    perror("pth_create error");
    return NULL;
  }

  pthread_t thread2;
  int pth2 = pthread_create(&thread2, NULL, MQ2, NULL);
  if (-1 == pth2)
  {
    perror("pth_create error");
    return NULL;
  }

  while (1) // 防止程序退出
  {
    if (-1 == *flag)
    {
      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);
    }
    break;
  }
  return 0;
}

/*
 *@brief 共享内存，用于线程间通信
 *@note 当共享内存中的值为-1时，退出环境检测
 */
int *shaerneric()
{
  key_t key = ftok("/home/xwp", 234); // 使用共享内存
  if (-1 == key)
  {
    printf("openftok error\n");
    return NULL;
  }
  int shmid = shmget(key, 4096, IPC_CREAT | 0666);
  if (-1 == shmid)
  {
    printf("shmget error\n");
    return NULL;
  }
  int *share = shmat(shmid, NULL, 0);
  if (NULL == share)
  {
    perror("shmat error");
    return NULL;
  }
  return share;
}