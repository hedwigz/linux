#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>

struct prinfo {
    pid_t parent_pid;
    pid_t pid;
    long state;
    uid_t uid;
    char comm[16];
    int level;
};