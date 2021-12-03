#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <math.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 4096

regex_t regex;
int reti;

double get_mhz_from_line(char *line)
{
  regmatch_t pmatch[2];
  int reti = regexec(&regex, line, 2, pmatch, 0);
  if (reti == REG_NOMATCH)
  {
    return 0;
  }
  if (reti)
  {
    char msgbuf[100];

    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
    printf("Regex match failed: %s\n", msgbuf);
    exit(1);
  }
  if (!reti) // we have a match
  {
    regmatch_t match = pmatch[1];
    int matchLen = match.rm_eo - match.rm_so;
    char *matchStr = malloc(matchLen + 1);
    sprintf(matchStr, "%.*s\n", matchLen, &line[match.rm_so]);
    matchStr[matchLen] = NULL;
    double mhz = strtod(matchStr, NULL);
    free(matchStr);
    return mhz;
  }
  return 0;
}

double get_mean_cpu_speed() {
  char *path = "/proc/cpuinfo";
  char line[MAX_LINE_LENGTH] = {0};
  unsigned int line_count = 0;

  FILE *file = fopen(path, "r");

  if (!file)
  {
    perror(path);
    return EXIT_FAILURE;
  }

  double sum = 0;
  int count = 0;
  /* Get each line until there are none left */
  while (fgets(line, MAX_LINE_LENGTH, file))
  {
    double mhz = get_mhz_from_line(line);
    if (mhz < 0.1)
    {
      continue;
    }
    // printf("got mhz: %f\n", mhz);
    count++;
    sum += mhz;
  }
  double mean = sum / count;
  fclose(file);
  return mean;
}

long long gethosttime(long long cycles) {
  double cpu_speed_mhz = get_mean_cpu_speed();
  long long cpu_speed = (long long)(cpu_speed_mhz);
  long long ns = 1000*(cycles / (cpu_speed_mhz));
  return ns;
}

long long gethosttime2(long long cycles) {
  double cpu_speed_mhz = 2600;
  long long cpu_speed = (long long)(cpu_speed_mhz);
  long long ns = 1000*(cycles / (cpu_speed_mhz));
  return ns;
}

static inline unsigned long long getcycles(void)
{
    unsigned long low, high;
    asm volatile ("rdtsc" : "=a" (low), "=d" (high));
    return ((low) | (high) << 32);
}

long double measure_getcycles() {
  long double N = 100000000;
  long long time1 = gethosttime(getcycles());
  for (int i=0; i < N/10; i++) {
    // since it is so fast, I want to eliminate as bit the overhead of looping
    // by doing some loop unrolling
    // update: the improvement was less than 1%
    getcycles();
    getcycles();
    getcycles();
    getcycles();
    getcycles();
    getcycles();
    getcycles();
    getcycles();
    getcycles();
    getcycles();
  }
  long long time2 = gethosttime(getcycles());
  printf("%lld %Lf\n", time2-time1, N);
  return ((long double)(time2-time1))/N;
}
double std(double arr[1000], int n, double mean) {
  double d_sum = 0;
  for (size_t i = 0; i < n; i++)
  {
    d_sum = pow(mean-arr[i], 2);
  }
  return sqrt(d_sum/n);
}

void measure_inner_loop() {
  int k = 0;
  long double time1, time2;
  double measure[1000];
  double mean = 0;
  for (size_t i = 0; i < 1000; i++)
  {
    time1 = gethosttime(getcycles());
    for (size_t j = 0; j < 100; j++)
    {
      k = i+j;
    }
    time2 = gethosttime(getcycles());
    measure[i] = time2-time1;
    mean += time2-time1;
  }

  double s = std(measure, 1000, mean);
  printf("the std is %.2fns and the mean is %.2fns\n", s, mean);
}

void measure_inner_loop2() {
  int k = 0;
  struct timeval t1, t2;
  double measure[1000];
  double mean = 0;
  for (size_t i = 0; i < 1000; i++)
  {
    
    gettimeofday(&t1, NULL);
    for (size_t j = 0; j < 100; j++)
    {
      k = i+j;
    }
    gettimeofday(&t2, NULL);
    measure[i] = 1000*(t2.tv_usec-t1.tv_usec);
    mean += measure[i];
  }

  double s = std(measure, 1000, mean);
  printf("the std is %.2fns and the mean is %.2fns\n", s, mean);
}

int main(int argc, char **argv)
{
  reti = regcomp(&regex, "cpu MHz\\s*:\\s\\([0-9]*\\.[0-9]*\\).*", 0);
  if (reti)
  {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }
  // double cpu_speed_mhz = get_mean_cpu_speed();
  // printf("cpu mean clock is %fMHz\n", cpu_speed_mhz);
  
  long long time1 = gethosttime(getcycles());
  // cpu got boosted from 2.6GHz to 4.4GHz
  long long time2 = gethosttime(getcycles());
  // time2 is much less than time1 do to clock boost and result is negative.
  printf("%lldns have passed\n",(time2-time1));

  printf("it takes %Lfns to perform getcycles()\n", measure_getcycles());

  measure_inner_loop();
  measure_inner_loop2();
}