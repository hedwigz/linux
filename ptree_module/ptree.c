#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/prinfo.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/oom.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amit Shani");
MODULE_DESCRIPTION("Learning how to build linux modules");
MODULE_VERSION("0.02");

extern struct task_struct init_task;

to_prinfo(struct task_struct *task, struct prinfo *pinfo)
{
  memcpy(pinfo->comm, task->comm, 16);
  pinfo->state = task->state;
  pinfo->pid = task->pid;
}

void traverse_processes() {
  struct task_struct *p;
	int pid;
  read_lock(&tasklist_lock);
	for_each_process(p) {
    if (!p->parent) {
      continue;
    }

	}
	read_unlock(&tasklist_lock);
}

int ptree(struct prinfo *buf, int *nr, int pid)
{
  struct task_struct *it = &init_task;
  if (pid == 0)
  {
    to_prinfo(&init_task, buf);
    return 0;
  }
  struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
  to_prinfo(p, buf);
  return 10;
}

static int __init ptree_init(void)
{
  struct prinfo pinfo;
  ptree(&pinfo, (int *)0, 849);
  printk(KERN_INFO "Hello, World! %s\n", pinfo.comm);
  return 0;
}

static void __exit ptree_exit(void)
{
  printk(KERN_INFO "Goodbye, World!\n");
}

module_init(ptree_init);
module_exit(ptree_exit);
