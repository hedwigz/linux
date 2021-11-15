#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/syscall.h>
struct prinfo {
    pid_t parent_pid;
    pid_t pid;
    long state;
    uid_t uid;
    char comm[16];
    int level;
};

// toy program to check the syscall
// honestly: it doesnt work (I get -EFAULT from the syscall)
// I probably need to fix something in the syscall but I don't have time (15.11 22:06) to recompile the syscall with
// the kernel and everything.
// I also didn't make the increasing by factor of two array (there's no reason if the syscall doesn't work).
// but, when I tested the module it worked well (not a 100% - I still had a small bug in the logic of looping the process tree)
// all in all, it was a really fun HW, but it took a lot of time and effort, which I sadly couldn't complete a 100%
// THANKS!
int main(int argc, char *argv[])
{
  int i=0;
  int nr = 1;
  struct prinfo* ps = malloc(5*sizeof(struct prinfo));
  printf("hello world!\n");
  int got = syscall(449, ps, &nr, 0);
  if (got <= 0) {
    perror("syscall");
  }
  printf("got: %d processes\n", got);
  for (i = 0; i< got; i++) {
    printf("%d,%s,%d,%d,%ld,%d\n", ps[i].level, ps[i].comm,
      ps[i].pid, ps[i].parent_pid, ps[i].state, ps[i].uid);
  }
}