#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include "jpeglib.h"
#include "link.h"

// extern int * plcd;
// 你要显示的那个jpg的图片路径名  plcd ->  是你映射的首地址
int jpg(char *pathname)
{

	// 1. 分配并初始化一个jpeg解压对象
	struct jpeg_decompress_struct dinfo; // 声明一个解压的对象

	struct jpeg_error_mgr jerr; // 声明一个出错信息的对象

	dinfo.err = jpeg_std_error(&jerr); // 初始化这个出错对象

	jpeg_create_decompress(&dinfo); // 初始化dinfo这个解压对象

	// 2. 指定要解压缩的图像文件

	FILE *fp;
	fp = fopen(pathname, "r");
	if (fp == NULL)
	{
		perror("fopen error");
		return -1;
	}

	jpeg_stdio_src(&dinfo, fp); // 指定要解压的图像文件

	// 3.	调用jpeg_read_header()获取图像信息
	jpeg_read_header(&dinfo, TRUE);

	// 4.	用于设置jpeg解压对象dinfo的一些参数。可采用默认参数

	/*5. 调用jpeg_start_decompress()启动解压过程


	调用jpeg_start_decompress函数之后，JPEG解压对象dinfo中
	下面这几个字段(成员变量)将会比较有用：
		dinfo.output_width: 	图像输出宽度，一行占多少个像素点
		dinfo.output_height:	图像输出高度，占多少行
		dinfo.output_components:  每个像素点的分量数，每个像素点占多少个字节
								3： R G B
								4：A R G B
				width * height * components


		在调用jpeg_start_decompress之后，往往需要为解压后的扫描线上的
		所有像素点分配存储空间：
			存一行： output_width * output_components
		*/
	jpeg_start_decompress(&dinfo);

	// 6. 读取一行或者多行扫描线上数据并处理，通常的代码是这样子的:

	unsigned char *buffer = malloc(dinfo.output_width * dinfo.output_components);

	// dinfo.output_scanline , 表示的意思是，已经扫描了多少行
	int x, y = 0;
	while (dinfo.output_scanline < dinfo.output_height)
	{
		y = dinfo.output_scanline;	 // 将要扫描第y行
		jpeg_read_scanlines(&dinfo,	 // 解压对象
												&buffer, // 保存解压后数据的二级指针,
												1				 // 读取多少行数据来解压
		);
		// dinfo.output_scanline + 1
		unsigned char *p = buffer;
		unsigned char a, r, g, b;
		int color;

		for (x = 0; x < dinfo.output_width; x++)
		{
			if (dinfo.output_components == 3)
			{
				a = 0;
			}
			else
			{
				a = *(p++);
			}
			r = *(p++);
			g = *(p++);
			b = *(p++);
			color = (a << 24 | r << 16 | g << 8 | b);

			//*(plcd + y * 800 + x) = color;
			DrawPoint(x, y, color);
		}
	}
	/*对扫描线的读取是按照从上到下的顺序进行的，也就是说图像最上方的扫描线最先
		被jpeg_read_scanlines()读入到存储空间中，紧接着是第二行扫描线，最后是
		图像底边的扫描线被读入到存储空间中去。*/

	// 7. 调用jpeg_finish_decompress()完成解压过程
	jpeg_finish_decompress(&dinfo);

	// 8. 调用jpeg_destroy_decompress释放jpeg解压对象dinfo
	jpeg_destroy_decompress(&dinfo);

	free(buffer);
}

/*
 *@brief 把jpeg格式图片的数据放入内存中
 *@param pic_pathname 图片路径名
 *@param node bmp图片颜色存放的节点，node->color 类型为int *
 */
int readjpg(const char *pathname, void *node)
{

	// 1. 分配并初始化一个jpeg解压对象
	struct jpeg_decompress_struct dinfo; // 声明一个解压的对象

	struct jpeg_error_mgr jerr; // 声明一个出错信息的对象

	dinfo.err = jpeg_std_error(&jerr); // 初始化这个出错对象

	jpeg_create_decompress(&dinfo); // 初始化dinfo这个解压对象

	// 2. 指定要解压缩的图像文件

	FILE *fp;
	fp = fopen(pathname, "r");
	if (fp == NULL)
	{
		perror("fopen error");
		return -1;
	}

	jpeg_stdio_src(&dinfo, fp); // 指定要解压的图像文件

	// 3.	调用jpeg_read_header()获取图像信息
	jpeg_read_header(&dinfo, TRUE);

	// 4.	用于设置jpeg解压对象dinfo的一些参数。可采用默认参数

	/*5. 调用jpeg_start_decompress()启动解压过程


	调用jpeg_start_decompress函数之后，JPEG解压对象dinfo中
	下面这几个字段(成员变量)将会比较有用：
		dinfo.output_width: 	图像输出宽度，一行占多少个像素点
		dinfo.output_height:	图像输出高度，占多少行
		dinfo.output_components:  每个像素点的分量数，每个像素点占多少个字节
								3： R G B
								4：A R G B
				width * height * components


		在调用jpeg_start_decompress之后，往往需要为解压后的扫描线上的
		所有像素点分配存储空间：
			存一行： output_width * output_components
		*/
	jpeg_start_decompress(&dinfo);

	// 6. 读取一行或者多行扫描线上数据并处理，通常的代码是这样子的:

	unsigned char *buffer = malloc(dinfo.output_width * dinfo.output_components);

	// dinfo.output_scanline , 表示的意思是，已经扫描了多少行
	int x, y = 0;
	// printf("w;%d h;%d\n", dinfo.output_width, dinfo.output_components);
	linknode *jp = node;
	jp->w = dinfo.output_width;
	jp->h = dinfo.output_height;
	jp->color = malloc(sizeof(int) * jp->h * jp->w);
	int *rgb = jp->color;
	while (dinfo.output_scanline < dinfo.output_height)
	{
		y = dinfo.output_scanline;	 // 将要扫描第y行
		jpeg_read_scanlines(&dinfo,	 // 解压对象
												&buffer, // 保存解压后数据的二级指针,
												1				 // 读取多少行数据来解压
		);
		// dinfo.output_scanline + 1
		unsigned char *p = buffer;
		unsigned char a, r, g, b;
		int color;
		for (x = 0; x < dinfo.output_width; x++)
		{
			if (dinfo.output_components == 3)
			{
				a = 0;
			}
			else
			{
				a = *(p++);
			}
			r = *(p++);
			g = *(p++);
			b = *(p++);
			color = (a << 24 | r << 16 | g << 8 | b);
			*rgb = color;
			rgb++;
			//*(plcd + y * 800 + x) = color;
		}
	}
	/*对扫描线的读取是按照从上到下的顺序进行的，也就是说图像最上方的扫描线最先
		被jpeg_read_scanlines()读入到存储空间中，紧接着是第二行扫描线，最后是
		图像底边的扫描线被读入到存储空间中去。*/

	// 7. 调用jpeg_finish_decompress()完成解压过程
	jpeg_finish_decompress(&dinfo);

	// 8. 调用jpeg_destroy_decompress释放jpeg解压对象dinfo
	jpeg_destroy_decompress(&dinfo);

	free(buffer);
}

/*
 *@brief 使用缓存中的颜色显示图片
 *@pic_pathname 保存的图片的路径
 *@param node 保存的图片的数据
 *@note 因为使用的是图片保存在内存中的颜色，所以我们不需要从硬盘上读取，所以响应速度就会更快了
 */
void DisplayJpeg(const char *pic_pathname, void *node)
{
	int x, y;
	linknode *p = node;
	int *rgb = p->color + p->h * p->w - 1; // 与bmp存储的颜色相反
	printf("正在显示 %s\n", pic_pathname);
	for (y = p->h - 1; y >= 0; y--)
	{
		for (x = 0; x < p->w; x++)
		{
			DrawPoint(x, y, *rgb);
			rgb--;
		}
	}
}