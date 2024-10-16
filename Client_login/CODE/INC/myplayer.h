#ifndef __MYPLAYER__H__
#define __MYPLAYER__H__

void CleanFifo(int fifofd);

void *MousTouch(void *arg);

int sosuovideo(const char *filepathname, void *L);

int myplayer();

void disbaifenbi(int num, int zicolor, int dicolor);

#endif