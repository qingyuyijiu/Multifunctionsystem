#ifndef _JPG_H_
#define _JPG_H_

int jpg(char *pathname);

void DisplayJpeg(const char *pic_pathname, void *node);

int readjpg(const char *pic_pathname, void *node);
#endif