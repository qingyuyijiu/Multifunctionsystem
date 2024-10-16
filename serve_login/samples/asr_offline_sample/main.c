#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../../include/mysql.h"
#include "../../include/Server.h"

int main(void)
{
  pthread_t pthreadfd;
  pthread_create(&pthreadfd, NULL, mysql, NULL);
  Server(NULL);
  return 0;
}
