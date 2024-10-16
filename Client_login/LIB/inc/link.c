#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "link.h"

static linknode *getp; // 全局变量，在getnext和getlast中使用
/*
 * @创建循环单链表的头节点
 */
linkhead *Creatlinkhead()
{
  linkhead *L = (linkhead *)malloc(sizeof(linkhead));
  L->first = NULL;
  L->rear = NULL;
  return L;
}

/*
 * @brief 添加路径名到循环单链表中
 * @param l 循环单链表的头节点
 * @param str 要传入的路径名
 * @note 一个简单的循环单链表
 * @note 使用尾插
 */
void addpathname(linkhead *l, char *str)
{
  if (!l)
  {
    return;
  }
  l->n = 1;
  linknode *p = malloc(sizeof(linknode));
  p->data = str; // str是malloc出来的空间
  p->next = NULL;
  p->last = NULL;
  if (NULL == l->first) // 如果链表中没有数据
  {
    l->first = p;
    l->rear = p;
    p->next = p;
    p->last = p;
  }
  else
  {
    l->rear->next = p; // 尾插，从尾部插入，所以rear指向最后一个进入的数据
    p->last = l->rear;
    l->rear = p;
    p->next = l->first;
    l->first->last = p;
  }
  return;
}

/*
 * @brief 获得下一个路径名
 * @param l 循环单链表的头节点
 * @param flag 0表示从头开始，1表示从上一次的位置开始
 * @return 返回下一个链表中指针
 * @note 一个简单的循环单链表
 * @note 注意：调用完getnext后，需要手动释放内存
 */
void *getnext(linkhead *l, int flag)
{
  if (!l)
    return NULL;
  if (flag == 0)
    getp = l->first;
  else
    getp = getp->next;
  if (getp)
    return getp;
  return NULL;
}

/*
 * @brief 获得上一个路径名
 * @param l 循环单链表的头节点
 * @param flag 0表示从头开始，1表示从上一次的位置开始
 * @return 返回下一个链表中指针
 * @note 一个简单的循环单链表
 * @note 注意：调用完getlast后，需要手动释放内存
 */
void *getlast(linkhead *l, int flag)
{
  if (!l)
    return NULL;
  if (flag == 0)
    getp = l->rear;
  else
    getp = getp->last;
  if (getp)
    return getp;
  return NULL;
}

/*
 * @brief 清空链表
 * @param l 循环单链表的头节点
 * @note 从头部开始删除，释放内存
 */
void cleanlink(linkhead *l) // 可能有问题
{
  if (!l)
    return;
  linknode *p;
  while (l->first) // l->first非空表示还有节点
  {
    p = l->first;
    if (l->rear == l->first) // 头和尾指向同一个节点，只有一个节点，删掉就没了
    {
      l->rear = NULL;
      l->first = NULL;
    }
    else // 有多个节点
    {
      l->first = l->first->next;
      l->rear->next = l->first;
      l->first->last = l->rear;
    }
    free(p->data);
    free(p->color);
    free(p);
  }
  return;
}

/*
 * @brief 销毁链表
 * @param l 循环单链表的头节点
 * @note 从头部开始删除，释放内存
 */
void Destroyleinkhead(linkhead **l)
{
  if (!l || !*l)
    return;
  cleanlink(*l);
  free(*l);
}