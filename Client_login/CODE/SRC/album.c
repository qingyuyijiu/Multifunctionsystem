#include <stdio.h>
#include "album.h"
#include "link.h"
#include "sosuopic.h"
#include "touch.h"
#include "lcd.h"
#include "albumface.h"
#include "BmpPicture.h"

/*
 *@brief 实现电子相册的功能,图片文件夹的路径为/source/picture
 *@author xiewanpi
 */
int album()
{
  linkhead *L = Creatlinkhead();
  printf("正在加载图片...\n");
  int r = sosuopic("/source/picture", L);
  if (-1 == r)
  {
    printf("搜索图片路径失败/n");
    return -1;
  }
  else
  {
    printf("所有图片已加载\n");
  }
  printf("进入相册\n");
  set_flag();
  AlbumInterface(L);
  Destroyleinkhead(&L);
  return 0;
}