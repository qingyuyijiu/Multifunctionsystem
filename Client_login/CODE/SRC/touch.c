#include "Touch.h"
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


/*
 *@brief 全局函数
 *@param touch_fd 触摸屏文件描述符
 */
static int Touch_fd = -1;

void TouchInit(void)
{
    Touch_fd = open("/dev/input/event0", O_RDWR);
    if (-1 == Touch_fd)
    {
        perror("open event0 error");
        exit(6);
    }
}

// 获取触摸屏方向
TouchDirection GetTouchDirection(int *x, int *y)
{
    // 维护在一个结构体
    struct input_event ev;
    int x0 = -1, y0 = -1;
    int x1 = -1, y1 = -1;
    while (1)
    {
        int r = read(Touch_fd, &ev, sizeof(ev));
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
        }
        if (0x01 == ev.type && BTN_TOUCH == ev.code && 0x01 == ev.value)
        {
            x0 = x1;
            y0 = y1;
        }
        else if (ev.type == 0x01 && ev.code == BTN_TOUCH && ev.value == 0)
        {
            if (x && y)
            {
                *x = x1 * 800 / 1024;
                *y = y1 * 480 / 595;
            }
            if (x0 == x1 && y0 == y1)
            {
                return TouchChick;
            }
            else if (x0 > 944 && (x0 - x1) > abs(y0 - y1))
            {
                return Touchreturn;
            }
            else if ((y0 - y1) > 0 && (y0 - y1) > abs(x0 - x1))
            {
                return TouchUp;
            }
            else if ((y0 - y1) < 0 && (y1 - y0) > abs(x0 - x1))
            {
                return TouchDown;
            }
            else if ((x0 - x1) > 0 && (x0 - x1) > abs(y0 - y1))
            {
                return TouchLeft;
            }
            else if ((x0 - x1) < 0 && (x1 - x0) > abs(y0 - y1))
            {

                return TouchRight;
            }
        }
    }
}

// 触摸屏销毁
void TouchDestroy(void)
{
    close(Touch_fd);
}
