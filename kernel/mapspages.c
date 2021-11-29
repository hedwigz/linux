#include <linux/prinfo.h>
#include <linux/uaccess.h>
#include <asm/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/audit.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/prinfo.h>
#include <linux/ptree.h>
#include <linux/slab.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>
#include <linux/limits.h>
#include <linux/pagewalk.h>
#include <linux/seq_file.h>


asmlinkage int mapspages(unsigned long start, unsigned long end, char __user *buf, size_t size)
{
  if (!access_ok(nr, sizeof(int))) {
    return -EFAULT;
  }
  if (*nr < 1) {
    return -EINVAL;
  }
  if (!access_ok(buf, (*nr)*sizeof(struct prinfo))) {
    return -EFAULT;
  }
  
  return 0;
}