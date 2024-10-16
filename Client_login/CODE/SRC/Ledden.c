#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "EnvirDetection.h"
#include "serial_init.h"

int led_fd = -1;

/*
 *@brief 初始化led
 */
int led_init()
{
  int fd = open("/dev/led_drv", O_RDWR);
  if (-1 == fd)
  {
    perror("open led_drv error");
    return -1;
  }
  led_fd = fd;
  return fd;
}

int get_led_status()
{
  return led_fd;
}