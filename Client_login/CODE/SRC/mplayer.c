#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <dirent.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include "fonts.h"
#include "BmpPicture.h"
#include "lcd.h"
#include "touch.h"
#include "link.h"
#include "myplayer.h"

linkhead *head = NULL;

void CleanFifo(int fifofd)
{
	// 将管道里面的信息全部清除
	char buf[1024];
	// 先将这个fifofd设置为非阻塞
	unsigned long flag = fcntl(fifofd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(fifofd, F_SETFL, flag);
	while (1)
	{
		int r = read(fifofd, buf, 1024);
		if (r <= 0)
		{
			break;
		}
	}
	// 变回阻塞
	flag = fcntl(fifofd, F_GETFL);
	flag &= ~O_NONBLOCK; // 去掉这个标志
	fcntl(fifofd, F_SETFL, flag);
}

void *MousTouch(void *arg)
{
	pthread_detach(pthread_self());
	// TouchInit();
	int x, y;
	int volume_step = 10;										 // 音量增加或减小的量
	int volume = -1;												 // 初始音量为-1，表示还没初始化
	int fd = open("/home/1.fifo", O_WRONLY); // 只写打开通道
	if (-1 == fd)
	{
		perror("fopen fifo1 error");
		return NULL;
	}
	int fd2 = open("/home/2.fifo", O_RDONLY); // 读取mplayer的视频流
	if (-1 == fd2)
	{
		perror("fopen fifo1 error");
		return NULL;
	}

	sem_t *semid = sem_open("/1.sem", 0); // 打开名信号量
	if (SEM_FAILED == semid)
	{
		perror("sem_open error");
		return NULL;
	}

	// 共享内存
	key_t key = ftok("/home/xwp", 123);
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
	int *flag = shmat(shmid, NULL, 0);
	if (NULL == flag)
	{
		perror("shmat error");
		return NULL;
	}
	while (1)
	{
		char buf[128] = {0}; // 开辟栈，每次使用完会自动释放
		if (-1 == volume)		 // 初始化音量
		{
			volume = 20; // 设置初始音量大小
			sprintf(buf, "volume %d 1\n", volume);
			printf("初始化音量： %d%%\n", volume * 100 / 50);
			int r = write(fd, buf, strlen(buf)); // 写入指令
			if (r != strlen(buf))
			{
				perror("write fifo error");
				return NULL;
			}
			continue;
		}
		TouchDirection touch = GetTouchDirection(&x, &y); // 获取触摸屏坐标
		printf("click x:%d y:%d\n", x, y);

		// 发送指令
		if (touch == TouchChick || x > 700 || y > 430) // 防误触
		{
			if (x >= 700 && x < 800 && y > 0 && y < 53) // 返回退出视频
			{
				sem_wait(semid);
				*flag = -1;
				sem_post(semid);
				sprintf(buf, "quit\n"); // 发送指令
				printf("写入指令：%s\n", buf);
				int r = write(fd, buf, strlen(buf)); // 写入指令
				if (r != strlen(buf))
				{
					perror("write fifo error");
					return NULL;
				}
				usleep(800000);
				break;
			}
			else if (x >= 700 && x < 800 && y > 405 && y < 480) // 暂停/开始
			{
				sem_wait(semid);
				if (*flag == 0 || flag == NULL)
					*flag = 1;
				else
					*flag = 0;
				sem_post(semid);
				sprintf(buf, "pause\n");
				printf("暂停/开始\n");
			}
			else if (x >= 700 && x < 800 && y > 70 && y < 130) // 音量变大
			{
				if (volume <= 10)
				{
					volume += 1;
				}
				else
					volume += volume_step;
				if (volume > 50)
					volume = 50;
				sprintf(buf, "volume %d 1\n", volume);
				printf("音量变大当前音量： %d%%\n", volume * 100 / 50);
			}
			else if (x >= 700 && x < 800 && y > 130 && y < 190) // 音量变小
			{
				if (volume <= 10)
				{
					volume -= 1;
				}
				else
					volume -= volume_step;
				if (volume < 0)
					volume = 0;
				sprintf(buf, "volume %d 1\n", volume);
				printf("音量变大当前音量： %d%%\n", volume * 100 / 50);
			}
			else if (x >= 700 && x < 800 && y > 200 && y < 265) // 回退
			{
				sem_wait(semid);
				*flag = 0;
				sem_post(semid);
				printf("快退10s\n");
				sprintf(buf, "seek %d\n", -10);
			}
			else if (x >= 700 && x < 800 && y > 305 && y < 355) // 快进
			{
				sem_wait(semid);
				*flag = 0;
				sem_post(semid);
				printf("快进10s\n");
				sprintf(buf, "seek %d\n", 10);
			}
			else if (y > 430 && y < 480 && x < 700) // 点击改变播放进度
			{
				sem_wait(semid);
				*flag = 0;
				if (x < 58) // x要从50开始
					x = 0;
				else if (x > 680)
					x = 600;
				else
					x -= 58;
				int process = x * 100 / 600; // 要播放的进度
				char buf1[44];							 // 之前的播放进度
				sprintf(buf1, "get_percent_pos\n");
				CleanFifo(fd2);												 // 清除管道里面的信息
				int r = write(fd, buf1, strlen(buf1)); // 写入指令
				if (r != strlen(buf1))
				{
					perror("write fifo error");
					break;
				}
				read(fd2, buf1, 44);
				int nexttime = atoi(&buf1[21]);

				sprintf(buf, "seek %d 1\n", process); // 发送指令
				r = write(fd, buf, strlen(buf));			// 写入指令
				if (r != strlen(buf))
				{
					perror("write fifo error");
					return NULL;
				}
				drawjuxing(48, 445, 620, 5, 0x0);
				drawjuxing(48, 460, 620, 5, 0x0);
				if (process > nexttime) // 进度条前进
				{
					drawjuxing(58, 450, 600 / 100 * process, 10, 0x0000ff);
					drawyuan(58 + 600 / 100 * process, 455, 10, 0xff);
				}
				else if (process <= nexttime) // 进度条回退
				{
					drawjuxing(600 / 100 * process + 58, 450, 600 - 600 / 100 * process, 10, 0x00);
					drawyuan(58 + 600 / 100 * process, 455, 10, 0xff);
				}
				sem_post(semid);
				continue;
			}
		}
		else if (touch == TouchRight) // 1倍数播放
		{
			printf("1倍数播放\n");
			sprintf(buf, "speed_set %d\n", 2);
		}
		else if (touch == TouchLeft) // 正常速度播放
		{
			printf("正常速度播放\n");
			sprintf(buf, "speed_set %d\n", 1);
		}
		else if (touch == TouchUp) // 上滑切换视频播放
		{
			printf("切换下一个视频 \n");
			linknode *node = getnext(head, 1); // 获得下一个视频地址
			printf("下一个视频%s\n", node->data);
			if (node == NULL)
			{
				printf("没有视频你看个der\n"); // 这里可以加个提示信息
				continue;
			}
			volume = -1;															 // 重置音量
			sprintf(buf, "loadfile %s\n", node->data); // 发送指令
			drawjuxing(48, 445, 620, 20, 0x00);
		}
		else if (touch == TouchDown) // 下滑切换视频播放
		{
			printf("切换上一个视频\n");
			linknode *node = getlast(head, 1); // 获得上一个视频地址
			if (node == NULL)
			{
				printf("没有视频你看个der\n"); // 这里可以加个提示信息
				continue;
			}
			volume = -1;															 // 重置音量
			sprintf(buf, "loadfile %s\n", node->data); // 发送指令
			drawjuxing(48, 445, 620, 20, 0x00);
		}
		else
		{
			continue;
		}
		printf("写入指令：%s\n", buf);
		int r = write(fd, buf, strlen(buf)); // 写入指令
		if (r != strlen(buf))
		{
			perror("write fifo error");
			return NULL;
		}
	}
	shmdt(flag);			// 释放共享内存
	sem_close(semid); // 关闭信号量
	close(fd);				// 关闭管道
	close(fd2);
	// TouchDestroy();
}

/*
 *@brief 搜索/source文件夹下的所有文件,把发现的需要的进行存储
 *@param filepathname 源文件夹路径
 *@return 成功返回0 失败返回-1
 *@note 这个函数的实现比较简单，主要是使用readdir()函数递归遍历源文件夹
 */
int sosuovideo(const char *filepathname, void *L)
{
	if (!L)
		return -1;
	linkhead *l = L;
	DIR *dp = opendir(filepathname);
	if (dp == NULL)
	{
		perror("opendir error");
		return -1;
	}
	struct dirent *dirt;
	while (dirt = readdir(dp))
	{
		if (!strcmp(dirt->d_name, ".") || !strcmp(dirt->d_name, ".."))
			continue;
		char *pathname = malloc(sizeof(filepathname) + sizeof(dirt->d_name));
		sprintf(pathname, "%s%s", filepathname, dirt->d_name);
		if (dirt->d_type == DT_REG) // 普通文件
		{
			// printf("路径%s是普通文件\n", pathname);
			if (!strcmp(pathname + strlen(pathname) - 4, ".mp4"))
			{
				// BmpPictureDisplay(pathname);
				addpathname(l, pathname);
				printf("路径%s加载成功\n", pathname); // 测试成功
			}
		}
		else if (dirt->d_type == DT_DIR) // 如果是一个目录
		{
			sosuovideo(pathname, l);
		}
	}
	return 0;
}

int myplayer()
{
	BmpPictureDisplay("/source/backgroundpic/mplyer.bmp");
	head = Creatlinkhead();
	printf("正在加载视频\n");
	sosuovideo("/mnt/udisk/video/", head); // 加载视频
	printf("视频加载成功\n");
	int mkf1 = mkfifo("/home/1.fifo", 0664); // 写入指令
	if (-1 == mkf1)
	{
		if (errno != EEXIST)
		{
			perror("mkfifo error");
			return -1;
		}
		printf("fifo exist,allow operate\n");
	}

	int mkf2 = mkfifo("/home/2.fifo", 0664); // 视频写入
	if (-1 == mkf2)
	{
		if (errno != EEXIST)
		{
			perror("mkfifo error");
			return -1;
		}
		printf("fifo exist,allow operate\n");
	}
	linknode *node = getnext(head, 0); // 获得第一个视频路径
	if (node == NULL)
	{
		printf("没有视频你看个der\n"); // 这里可以加个提示信息
		return -1;
	}

	pid_t pid = fork();
	if (pid == 0) // 子进程跑mplay的代码
	{
		int fd = open("/home/2.fifo", O_WRONLY); // 这个管道是mplayer用来写的
		if (-1 == fd)
		{
			perror("open 1.fifo error");
			return -2;
		}
		char command[1024];																																											 // 定义一个足够大的字符串来存储命令
		sprintf(command, "mplayer -slave -quiet -input file=/home/1.fifo -zoom -x 700 -y 430 %s &", node->data); // 构建命令字符串
		printf("command:%s\n", command);
		dup2(fd, 1);						 // 清零标准输出，将mplayer的输出重定向到管道
		int r = system(command); // 运行mplayer
		if (-1 == r)
		{
			perror("execlp error");
			return -2;
		}
	}
	else if (pid > 0) // 父进程操作mplayer
	{
		sem_t *semid = sem_open("/1.sem", O_CREAT, 666, 1); // 添加有名信号量
		if (SEM_FAILED == semid)
		{
			perror("sem_open error");
			return -5;
		}
		sem_init(semid, 0, 1);

		// 共享内存
		key_t key = ftok("/home/xwp", 123);
		if (-1 == key)
		{
			printf("openftok error\n");
			return -1;
		}
		int shmid = shmget(key, 4096, IPC_CREAT | 0666);
		if (-1 == shmid)
		{
			printf("shmget error\n");
			return -1;
		}
		int *flag = shmat(shmid, NULL, 0);
		if (NULL == flag)
		{
			perror("shmat error");
			return -1;
		}
		*flag = 0;
		pthread_t pthread1;
		int pth1 = pthread_create(&pthread1, NULL, MousTouch, NULL); // 开个线程打开触摸屏
		if (-1 == pth1)
		{
			perror("pth_create error");
			return -3;
		}

		int fd2 = open("/home/2.fifo", O_RDONLY); // 读取mplayer的视频流
		if (-1 == fd2)
		{
			perror("fopen fifo1 error");
			return -4;
		}

		int fd = open("/home/1.fifo", O_WRONLY);
		if (-1 == fd)
		{
			perror("fopen fifo1 error");
			return -4;
		}

		// drawjuxing(51, 449, 602, 12, 0x0);
		drawjuxing(48, 445, 620, 20, 0x00);
		int nexttime = -1;
		while (1) // 每0.5秒读取一次进度条
		{
			sem_wait(semid);
			// printf("flag:%d\n", *flag);
			if (-1 == *flag)
			{
				// sleep(1); // 睡眠0.5秒,等待子程序和子线程退出
				break;
			}
			if (0 == *flag || NULL == flag) // flag=1表示在暂停状态
			{
				char buf[44];
				sprintf(buf, "get_percent_pos\n");
				CleanFifo(fd2);											 // 清除管道里面的信息
				int r = write(fd, buf, strlen(buf)); // 写入指令
				if (r != strlen(buf))
				{
					perror("write fifo error");
					break;
				}
				read(fd2, buf, 44);
				int d = atoi(&buf[21]);
				drawjuxing(48, 445, 620, 5, 0x0);
				drawjuxing(48, 460, 620, 5, 0x0);
				disbaifenbi(d, 0x00, 0xffffff);
				if (d > nexttime) // 进度条前进,,使用共享内存和锁解决不能暂停的问题
				{
					drawjuxing(58, 450, 600 / 100 * d, 10, 0x0000ff);
					drawyuan(58 + 600 / 100 * d, 455, 10, 0xff);
					nexttime = d;
				}
				else if (d <= nexttime) // 进度条回退
				{
					drawjuxing(600 / 100 * d + 58, 450, 600 - 600 / 100 * d, 10, 0x00);
					drawyuan(58 + 600 / 100 * d, 455, 10, 0xff);
					nexttime = d;
				}
			}
			sem_post(semid);
			usleep(500000); // 睡眠0.1秒
		}
		shmdt(flag);
		close(fd); // 关闭管道
		close(fd2);
		sem_close(semid);
	}
	return 0;
}

/*
 *@brief 显示进度条百分比
 *@param num 百分比
 *@param zicolor 字体颜色
 *@param dicolor 底色
 *@note 这个函数的实现比较简单，主要是使用zimoDisplay()函数绘制百分比
 */
void disbaifenbi(int num, int zicolor, int dicolor)
{
	char baifenhao[] = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x10, 0x6C, 0x10,
			0x44, 0x20, 0xC6, 0x20, 0xC6, 0x40, 0xC6, 0x40, 0xC6, 0x40, 0xC6, 0x80, 0xC6, 0x80, 0x44, 0x80,
			0x6D, 0x38, 0x39, 0x6C, 0x02, 0x44, 0x02, 0xC6, 0x02, 0xC6, 0x04, 0xC6, 0x04, 0xC6, 0x08, 0xC6,
			0x08, 0xC6, 0x08, 0x44, 0x10, 0x6C, 0x10, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"%",0*/
	};
	drawjuxing(0, 440, 32, 32, dicolor);
	zimoDisplay(16, 32, 32, 440, baifenhao, zicolor, dicolor);
	shuzizimodisplay(16, 32, 0, 440, num, zicolor, dicolor);
	return;
}
