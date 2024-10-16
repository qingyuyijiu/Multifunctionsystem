#include <stdio.h>
#include "sqlite3.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

sqlite3 *ppDb;

/*
 *@brief 查找数据库中的所有信息
 */
void SelectAll(void)
{
    // 编译字节码
    const char *sql = "SELECT * FROM account;";
    sqlite3_stmt *ppStmt; // 字节码对象
    int r = sqlite3_prepare_v2(
        ppDb,        /* 数据库连接对象 */
        sql,         /* sql语句 */
        strlen(sql), /* 你需要编译的sql语句有多少个字节 */
        &ppStmt,     /* 二级指针，将一级指针变量的地址传入之后，调用
                                   这个函数成功，外面的一级指针就会指向一个可用的字节码对象 */
        NULL         /* OUT: Pointer to unused portion of zSql */
    );
    if (SQLITE_OK != r)
    {
        printf("字节码编译失败:%s\n", sqlite3_errmsg(ppDb));
        exit(2);
    }
    int flag = 0;
    // 这个玩意儿很有可能会有结果
    while (1)
    {
        if (0 == flag)
        {
            printf("|---手机号--|---账号---|---密码---|\n");
            flag = 1;
        }
        r = sqlite3_step(ppStmt);
        // 判断  是否存在结果
        if (SQLITE_DONE == r) // 没有结果了
        {
            break;
        }
        else if (SQLITE_ROW == r)
        {
            printf("|%-11s|%-10s|%-10s|\n", sqlite3_column_text(ppStmt, 0), sqlite3_column_text(ppStmt, 1), sqlite3_column_text(ppStmt, 2));
        }
        else
        {
            printf("字节码执行失败:%s\n", sqlite3_errmsg(ppDb));
            exit(1);
        }
    }
    sqlite3_finalize(ppStmt);
}

/*
 *@brief 添加数据到数据库中
 */
void AddData(void)
{
    char geterrbuf[1024] = {0};
    const char *sql = "INSERT INTO account(phone,account,password)VALUES(?,?,?);";
    sqlite3_stmt *ppStmt; // 字节码对象
    int r = sqlite3_prepare_v2(
        ppDb,        /* 数据库连接对象 */
        sql,         /* sql语句 */
        strlen(sql), /* 你需要编译的sql语句有多少个字节 */
        &ppStmt,     /* 二级指针，将一级指针变量的地址传入之后，调用
                                   这个函数成功，外面的一级指针就会指向一个可用的字节码对象 */
        NULL         /* OUT: Pointer to unused portion of zSql */
    );
    if (SQLITE_OK != r)
    {
        printf("字节码编译失败:%s\n", sqlite3_errmsg(ppDb));
        exit(2);
    }
    while (1)
    {
        char phone[14] = {0};
        char account[14] = {0};
        char password[14] = {0};
        printf("请输入手机号(输入:' #'退出):\n");
        printf("\t手机号:");
        fflush(stdout);
        while (scanf("%s", phone) != 1)
        {
            printf("\tid输入有误，请重新输入:");
            fflush(stdout);
            fgets(geterrbuf, 1024, stdin); // 将错误的数据读走不要了
        }
        if (!strncmp(phone, "#", 2))
        {
            break;
        }
        getchar();

        printf("\t账号:");
        fflush(stdout);
        while (scanf("%s", account) != 1)
        {
            printf("\t账号输入有误，请重新输入:");
            fflush(stdout);
            fgets(geterrbuf, 1024, stdin); // 将错误的数据读走不要了
        }
        getchar();

        printf("\t密码:");
        fflush(stdout);
        while (scanf("%s", password) != 1)
        {
            printf("\t密码输入有误，请重新输入:");
            fflush(stdout);
            fgets(geterrbuf, 1024, stdin); // 将错误的数据读走不要了
        }
        getchar();

        // 绑定值到字节码
        sqlite3_bind_text(ppStmt, 1, phone, strlen(phone), NULL);       // 绑定int
        sqlite3_bind_text(ppStmt, 2, account, strlen(account), NULL);   // 绑定int
        sqlite3_bind_text(ppStmt, 3, password, strlen(password), NULL); // 绑定int

        // 执行字节码对象
        r = sqlite3_step(ppStmt);
        sqlite3_reset(ppStmt);
    }
    sqlite3_finalize(ppStmt); // 销毁字节码对象
}

/*
 *@brief 用户注册时添加，账号随机
 */
char *User_Add(char *phone, char *password)
{
    char geterrbuf[1024] = {0};
    const char *sql = "INSERT INTO account(phone,account,password)VALUES(?,?,?);";
    sqlite3_stmt *ppStmt; // 字节码对象
    int r = sqlite3_prepare_v2(
        ppDb,        /* 数据库连接对象 */
        sql,         /* sql语句 */
        strlen(sql), /* 你需要编译的sql语句有多少个字节 */
        &ppStmt,     /* 二级指针，将一级指针变量的地址传入之后，调用
                                   这个函数成功，外面的一级指针就会指向一个可用的字节码对象 */
        NULL         /* OUT: Pointer to unused portion of zSql */
    );
    if (SQLITE_OK != r)
    {
        printf("字节码编译失败:%s\n", sqlite3_errmsg(ppDb));
        exit(2);
    }

    char *account = calloc(14, sizeof(char));
    srand((unsigned int)time(NULL));
    unsigned long num = (rand() % 10000000000);
    if (num < 0)
        num = -num;
    num += 1000000000;
    printf("num%ld\n", num);
    sprintf(account, "%ld", num);
    printf("添加phone:%s,account:%s,password:%s到数据库中\n", phone, account, password);
    // 绑定值到字节码
    sqlite3_bind_text(ppStmt, 1, phone, strlen(phone), NULL);       // 绑定int
    sqlite3_bind_text(ppStmt, 2, account, strlen(account), NULL);   // 绑定int
    sqlite3_bind_text(ppStmt, 3, password, strlen(password), NULL); // 绑定int

    // 执行字节码对象
    r = sqlite3_step(ppStmt);
    // 后面继续执行的时候我们需要将绑定的值擦除
    // 回到编译好的时候的样子  这个空格里面填入的值就被擦除了
    sqlite3_reset(ppStmt);

    sqlite3_finalize(ppStmt); // 销毁字节码对象
    return account;
}

/*
 *@brief 打开或者创建一个数据库
 *@param dbFileName 为数据库路径
 */
void Sqlite3Init(const char *dbFileName)
{
    int r = sqlite3_open(dbFileName, &ppDb);
    if (SQLITE_OK != r) // 有问题了
    {
        printf("数据库打开失败了:%s\n", sqlite3_errmsg(ppDb));
        exit(1);
    }
    // printf("打开数据库成功了\n");

    // 准备一个SQL语句
    const char *sql = "CREATE TABLE account(phone text not null,account text primary key,password text not null);";

    char *errmsg = NULL;
    r = sqlite3_exec( // 一步到位操作接口  主要用作查找  但是其他的也是可以用的
                      //
        ppDb,         /* An open database */
        sql,          /* SQL to be evaluated */
        NULL,         /* Callback function */
        NULL,         /* 1st argument to callback */
        &errmsg       /* Error msg written here */
    );
    if (SQLITE_OK != r)
    {
        printf("sqlite3_exec error:%s\n", errmsg);
    }
    printf("打开数据库成功了\n");
    return;
}

/*
 *@brief 关闭数据库
 */
void Sqlite3Destory(void)
{
    // 关闭数据库连接
    sqlite3_close(ppDb);
}

/*
 *@brief 账号密码验证
 *@param account 为用户账号
 *@param password 为用户密码
 */
int Cmp_Accountpassword(char *account, char *password)
{
    char geterrbuf[1024] = {0};
    // 编译字节码
    const char *sql = "SELECT * FROM account where account =?;";
    sqlite3_stmt *ppStmt; // 字节码对象
    int r = sqlite3_prepare_v2(
        ppDb,        /* 数据库连接对象 */
        sql,         /* sql语句 */
        strlen(sql), /* 你需要编译的sql语句有多少个字节 */
        &ppStmt,     /* 二级指针，将一级指针变量的地址传入之后，调用
                                   这个函数成功，外面的一级指针就会指向一个可用的字节码对象 */
        NULL         /* OUT: Pointer to unused portion of zSql */
    );
    if (SQLITE_OK != r)
    {
        printf("字节码编译失败:%s\n", sqlite3_errmsg(ppDb));
        exit(2);
    }
    printf("******************************************************************************\n");
    // 绑定值到字节码
    sqlite3_bind_text(ppStmt, 1, account, strlen(account), NULL);
    // 执行字节码对象
    r = sqlite3_step(ppStmt);

    if (SQLITE_DONE == r) // 没有结果了
    {
        printf("没有找到该用户\n");
        sqlite3_finalize(ppStmt);
        return -1;
    }
    else if (SQLITE_ROW == r)
    {
        const char *sqlpassword = sqlite3_column_text(ppStmt, 2);
        printf("account:%s\tpassword:%s\n", sqlite3_column_text(ppStmt, 1), sqlpassword);
        if (!strcmp(sqlpassword, password)) // 账号存在且密码一致
        {
            sqlite3_finalize(ppStmt);
            return 0;
        }
        sqlite3_finalize(ppStmt);
        return -2;
    }
}

int Deletaccount()
{
    char geterrbuf[1024] = {0};
    // 编译字节码
    const char *sql = "delete FROM account where account =?;";
    sqlite3_stmt *ppStmt; // 字节码对象
    int r = sqlite3_prepare_v2(
        ppDb,        /* 数据库连接对象 */
        sql,         /* sql语句 */
        strlen(sql), /* 你需要编译的sql语句有多少个字节 */
        &ppStmt,     /* 二级指针，将一级指针变量的地址传入之后，调用
                                   这个函数成功，外面的一级指针就会指向一个可用的字节码对象 */
        NULL         /* OUT: Pointer to unused portion of zSql */
    );
    if (SQLITE_OK != r)
    {
        printf("字节码编译失败:%s\n", sqlite3_errmsg(ppDb));
        exit(2);
    }
    while (1)
    {
        char account[14] = {0};
        printf("请输入要删除的账号(输入:' #'退出):\n");
        printf("\t账号:");
        fflush(stdout);
        while (scanf("%s", account) != 1)
        {
            printf("\tid输入有误，请重新输入:");
            fflush(stdout);
            fgets(geterrbuf, 1024, stdin); // 将错误的数据读走不要了
        }
        if (!strncmp(account, "#", 2))
        {
            break;
        }
        getchar();
        // 绑定值到字节码
        sqlite3_bind_text(ppStmt, 1, account, strlen(account), NULL);
        // 执行字节码对象
        r = sqlite3_step(ppStmt);
        if (SQLITE_DONE == r)
        {
            printf("删除成功\n");
        }
        sqlite3_reset(ppStmt); // 擦除
    }
    sqlite3_finalize(ppStmt);
}

/*
 *@brief 在终端输入命令执行一些数据库对应的操作
 */
void *mysql(void *arg)
{
    // 打开数据库连接
    Sqlite3Init("./MySql/account.db"); // 数据库初始化
    char geterrbuf[1024];
    int a;
    while (1)
    {
        printf("1:增加数据 2:查找所有的结果 3.删除账号 0:退出系统->");
        fflush(stdout);
        if (scanf("%d", &a) != 1)
        {
            printf("\t输入有误，请重新输入\n");
            fflush(stdout);
            fgets(geterrbuf, 1024, stdin); // 将错误的数据读走不要了
            continue;
        }
        switch (a)
        {
        case 0:
            goto ExitSystem; // 退出系统
        case 1:
            AddData();
            break;
        case 2:
            SelectAll();
            break;
        case 3:
            Deletaccount();
            break;
        default:
            printf("输入有误，重新来一次\n");
            break;
        }
    }

ExitSystem:
    Sqlite3Destory();

    return 0;
}
