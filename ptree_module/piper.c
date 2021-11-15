
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int pipe_and_exec(char *programs[], int count)
{
  if (count == 0)
  {
    char line[1024];
    while (fgets(line, 1024, stdin) != NULL)
    {
      printf("%s", line);
    }
    return EXIT_SUCCESS;
  }
  pid_t pid;
  int fd[2];
  if (pipe(fd))
  {
    perror("pipe");
    return EXIT_FAILURE;
  }

  if ((pid = fork()) < 0)
  {
    perror("fork");
    return EXIT_FAILURE;
  }
  else if (!pid)
  { /* child */
    dup2(fd[0], STDIN_FILENO);
    return pipe_and_exec(&programs[1], count - 1);
  }
  else
  { /* parent */
    char *args[] = {programs[0], NULL};
    dup2(fd[1], STDOUT_FILENO);
    execvp(args[0], args);
    perror("execvp");
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  int count = 3;
  char *pgms[] = {"/usr/bin/ls", "/usr/bin/less", "/usr/bin/cat"};
  pid_t child;
  if ((child = fork()) == 0)
  {
    int res = pipe_and_exec(pgms, count);
    return res;
  }
  wait(NULL);
  printf("wait is over, exiting\n");
  return EXIT_SUCCESS;
}