#ifndef __LINK__H_
#define __LINK__H_

/*
 *@brief 使用双向循环链表
 */
typedef void *Mylinkdata; // 定义链表中保存的数据类型

typedef struct linknode // 链表节点结构体
{
  Mylinkdata data;
  struct linknode *next;
  struct linknode *last;
  int *color;
  int w;
  int h;
} linknode;

typedef struct linkhead // 链表头节点结构体
{
  int n;
  linknode *first;
  linknode *rear;
} linkhead;

/*
 *@brief 创建链表头
 */
linkhead *Creatlinkhead();

/*
 *@brief 销毁链表
 */
void Destroyleinkhead(linkhead **l);

/*
 *@brief 向链表中添加数据
 */
void addpathname(linkhead *l, char *str);

/*
 *@brief 获得链表中下一个数据
 */
void *getnext(linkhead *l, int flag);

/*
 *@brief 获得链表中下一个数据
 */
void *getlast(linkhead *l, int flag);

/*
 *@brief 清空链表
 */
void cleanlink(linkhead *l);

#endif