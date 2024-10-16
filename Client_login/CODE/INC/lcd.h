#ifndef __LCD__H_
#define __LCD__H_

void LcdInit(void);

void drawjuxing(int x0, int y0, int w, int h, int rgb);

void drawyuan(int x0, int y0, int r, int rgb);

void DrawPoint(int x, int y, unsigned int color);

void close_lcd();

void displaypicyuan(void *node);

void displaypicyuanbian(void *node);

void displayjianbian(void *node);

void displayjianbianxia(void *node);

void displaybaiye(void *node);

void DrawLinehen(int x, int y, int x1);

void DrawLineshu(int x, int y, int y1);

void cleanscreen();

void delay_ms(int s);

void DrawLine(int x0, int y0, int x1, int y1, int bit, int color);

void drawfaningbrush(int x0, int y0, int r, int rgb);

void tofabrush(int x0, int y0, int r, int rgb);

void penqian(int x0, int y0, int r, int rgb);

#endif