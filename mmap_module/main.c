#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <limits.h>

int main(int argc, char *argv[])
{
  int size = 8196;
  char * buf = malloc(size);
  // sys_mapspages(unsigned long start, unsigned long end, char __user *buf, size_t size)
  int got = syscall(450, 0, INT_MAX, buf, size);
  printf("got: %d", got);
  // int got = ptree(ps, &nr, 0);
  printf("%s\n", buf);
}