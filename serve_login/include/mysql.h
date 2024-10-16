#ifndef __MYSQL__H_
#define __MYSQL__H_

void AddData(void);

void Sqlite3Init(const char *dbFileName);

void Sqlite3Destory(void);

int Cmp_Accountpassword(char *account, char *password);

void *mysql(void *arg);

void SelectAll(void);

char *User_Add(char *phone, char *password);

#endif