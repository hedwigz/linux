#include <sys/types.h>
#include <kernel/prinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  int nr = 100;
  struct prinfo* ps = malloc(nr*sizeof(struct prinfo));
  int got = ptree(ps, &nr, 0);
  for (i = 0; i< got; i++) {
    printk(KERN_INFO "%d,%s,%d,%d,%ld,%d\n", ps[i].level, ps[i].comm,
      ps[i].pid, ps[i].parent_pid, ps[i].state, ps[i].uid);
  }
}