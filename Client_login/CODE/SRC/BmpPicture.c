#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "BmpPicture.h"
#include "link.h"
#include "lcd.h"

/*
 *@brief 解析bmp图片并保存在缓存区中
 *@param pic_pathname 图片路径名
 *@param node bmp图片颜色存放的节点，node->color 类型为int *
 *@return void
 */
void readBmp(const char *pic_pathname, void *node)
{
    int fd = open(pic_pathname, O_RDONLY);
    if (-1 == fd)
    {
        perror("打开图片失败！");
        printf("图片路径:%s\n", pic_pathname);
        return;
    }

    char bm[2];
    lseek(fd, 0x00, SEEK_SET);
    read(fd, bm, 4);
    if (bm[0] != 'B' || bm[1] != 'M')
    {
        perror("不是BMP图片");
        return;
    }

    unsigned int w, h;
    short d;
    lseek(fd, 0x12, SEEK_SET); // 读bmp图片信息,宽度
    read(fd, &w, 4);
    lseek(fd, 0x16, SEEK_SET); // 高度
    read(fd, &h, 4);
    lseek(fd, 0x1C, SEEK_SET); // 位数
    read(fd, &d, 2);

    int fill = 0;       // 填充字节
    linknode *p = node; // 此处定义的节点用于保存颜色
    fill = (4 - (w * (d / 8) % 4)) % 4;
    unsigned char *buf = malloc((w * d / 8 + fill) * h);
    p->color = malloc(sizeof(unsigned int) * w * h); // p->color的类型为int*
    p->w = w;                                        // 保存宽度
    p->h = h;                                        // 保存高度
    lseek(fd, 0x36, SEEK_SET);
    read(fd, buf, (w * d / 8 + fill) * h);
    char a = 0, r, g, b;
    int i = 0;
    unsigned int color;
    int x, y;
    int *rgb = p->color;
    for (y = h - 1; y >= 0; y--)
    {
        for (x = 0; x < w; x++)
        {
            b = buf[i++];
            g = buf[i++];
            r = buf[i++];
            if (32 == d)
            {
                a = buf[i++];
            }
            color = a << 24 | r << 16 | g << 8 | b;
            *rgb = color; // 保存颜色到内存
            rgb++;
        }
        i += fill;
    }
    close(fd);
}

/*
 *@brief 使用缓存中的颜色显示图片
 *@pic_pathname 保存的图片的路径
 *@param node 保存的图片的数据
 *@note 因为使用的是图片保存在内存中的颜色，所以我们不需要从硬盘上读取，所以响应速度就会更快了
 */
void DisplayBmp(const char *pic_pathname, void *node)
{
    int x, y;
    linknode *p = node;
    int *rgb = p->color;
    printf("正在显示 %s\n", pic_pathname);
    for (y = p->h - 1; y >= 0; y--)
    {
        for (x = 0; x < p->w; x++)
        {
            DrawPoint(x, y, *rgb);
            rgb++;
        }
    }
}

/*
 *@显示bmp图片
 */
void BmpPictureDisplay(const char *pic_pathname)
{
    int fd = open(pic_pathname, O_RDONLY);
    if (-1 == fd)
    {
        perror("打开图片失败！");
        printf("图片路径:%s\n", pic_pathname);
        return;
    }

    char bm[2];
    lseek(fd, 0x00, SEEK_SET);
    read(fd, bm, 4);
    if (bm[0] != 'B' || bm[1] != 'M')
    {
        perror("不是BMP图片");
        return;
    }

    unsigned int w, h;
    short d;
    lseek(fd, 0x12, SEEK_SET); // 读bmp图片信息,宽度
    read(fd, &w, 4);
    lseek(fd, 0x16, SEEK_SET); // 高度
    read(fd, &h, 4);
    lseek(fd, 0x1C, SEEK_SET); // 位数
    read(fd, &d, 2);

    int fill = 0;
    fill = (4 - (w * (d / 8) % 4)) % 4;
    unsigned char *buf = malloc((w * d / 8 + fill) * h);

    lseek(fd, 0x36, SEEK_SET);
    read(fd, buf, (w * d / 8 + fill) * h);
    char a = 0, r, g, b;
    int i = 0;
    unsigned int color;
    int x, y;
    for (y = h - 1; y >= 0; y--)
    {
        for (x = 0; x < w; x++)
        {
            b = buf[i++];
            g = buf[i++];
            r = buf[i++];
            if (32 == d)
            {
                a = buf[i++];
            }
            color = a << 24 | r << 16 | g << 8 | b;
            DrawPoint(x, y, color);
        }
        i += fill;
    }
    free(buf);
    close(fd);
}

/*
 *@brief 显示图片到屏幕中心
 */
void DispictoCenter(const char *pathname)
{
    int fd = open(pathname, O_RDONLY);
    if (-1 == fd)
    {
        perror("打开图片失败！");
        printf("图片路径:%s\n", pathname);
        return;
    }

    char bm[2];
    lseek(fd, 0x00, SEEK_SET);
    read(fd, bm, 4);
    if (bm[0] != 'B' || bm[1] != 'M')
    {
        perror("不是BMP图片");
        return;
    }

    unsigned int w, h;
    short d;
    lseek(fd, 0x12, SEEK_SET); // 读bmp图片信息,宽度
    read(fd, &w, 4);
    lseek(fd, 0x16, SEEK_SET); // 高度
    read(fd, &h, 4);
    lseek(fd, 0x1C, SEEK_SET); // 位数
    read(fd, &d, 2);

    int fill = 0;
    fill = (4 - (w * (d / 8) % 4)) % 4;
    unsigned char *buf = malloc((w * d / 8 + fill) * h);

    lseek(fd, 0x36, SEEK_SET);
    read(fd, buf, (w * d / 8 + fill) * h);
    char a = 0, r, g, b;
    int i = 0;
    unsigned int color;
    int x, y;
    int x0 = 400, y0 = 240; // 屏幕中心
    for (y = y0 + h / 2; y > y0 - h / 2; y--)
    {
        for (x = x0 - w / 2; x < x0 + w / 2; x++)
        {
            b = buf[i++];
            g = buf[i++];
            r = buf[i++];
            if (32 == d)
            {
                a = buf[i++];
            }
            color = a << 24 | r << 16 | g << 8 | b;
            DrawPoint(x, y, color);
        }
        i += fill;
    }
    free(buf);
    close(fd);
    return;
}
