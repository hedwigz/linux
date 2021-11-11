#include <linux/prinfo.h>

#ifndef PTREE_SYSCALL_H
#define PTREE_SYSCALL_H
typedef int (*ptree_func)(struct prinfo *buf, int *nr, int pid);
int register_ptree(ptree_func func);
void unregister_ptree(ptree_func func);
#endif
