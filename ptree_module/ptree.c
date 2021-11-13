#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/prinfo.h>
#include <linux/ptree.h>
#include <linux/slab.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>
#include <linux/limits.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amit Shani");
MODULE_DESCRIPTION("Learning how to build linux modules");
MODULE_VERSION("0.02");

extern struct task_struct init_task;

void to_prinfo(struct task_struct *task, struct prinfo *pinfo)
{
	memcpy(pinfo->comm, task->comm, 16);
	pinfo->state = task->state;
	pinfo->pid = task->pid;
	// pinfo->level = task->level;
}

// void traverse_processes(int *nr, pid_t parent)
// {
// 	struct task_struct *p;
// 	int pid;
// 	read_lock(&tasklist_lock);
// 	for_each_process (p) {
// 		if (!p->parent) {
// 			continue;
// 		}
// 		if (p->parent->pid != parent) {
// 		}
// 	}
// 	read_unlock(&tasklist_lock);
// }

struct task_struct *get_p(int pid)
{
	if (pid == 0) {
		return &init_task;
	}
	return pid_task(find_vpid(pid), PIDTYPE_PID);
}

int get_childs(pid_t root_pid, int * pids, int n) {
  int i = 0;
  struct task_struct *p;
  read_lock(&tasklist_lock);
  for_each_process(p) {
    // limit reached?
    if (i >= n) {
      break;
    }
    // check if process has parent
    if (!p->parent) {
      continue;
    }
    // is parent our root_pid?
    if (p->parent->pid == root_pid) {
      pids[i] = p->pid;
      ++i;
      continue;
    }
  }
  read_unlock(&tasklist_lock);
  // got:
  return i;
}

int ptree(struct prinfo *buf, int *nr, int pid)
{
  // n > 0 (syscall checks)
	int n = *nr;
	// count how many processes we got
	int got = 0;

	// allocate array of PIDs
	pid_t *pids;
	pids = kmalloc_array(n, sizeof(pid_t), GFP_KERNEL);
	if (!pids)
		return -ENOMEM;
  pids[0] = pid;

  int i = 0;
  int root_idx = 0;
	do {
    pid_t root_pid = pids[root_idx];
    printk(KERN_INFO "Looking for child processes of pid: %d\n", root_pid);
    ++root_idx;
    ++i;
    i += get_childs(root_pid, &pids[i], n-i);
	} while (root_idx <= i && i < n);
	got = i;
  printk(KERN_INFO "got total of %d processes:\n", got);
  return got;
  // now I need to list the items...
	// for (i = 0; i < got; i++) {

	// }

	// if (pid == 0)
	// {
	//   to_prinfo(&init_task, buf);
	//   return 0;
	// }
	// struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
	// to_prinfo(p, buf);
	// return 10;
}

static int __init ptree_init(void)
{
	printk(KERN_INFO "Hello, World!\n");
	int reg_err = register_ptree(&ptree);
	// struct prinfo pinfo;
	// ptree(&pinfo, (int *)0, 849);
	if (reg_err == 0) {
		printk(KERN_INFO "ptree func registered successfully!\n");
	} else {
		printk(KERN_ERR "failed to register ptree func %d\n", reg_err);
	}

	return reg_err;
}

static void __exit ptree_exit(void)
{
	unregister_ptree(&ptree);
	printk(KERN_INFO "Goodbye, World!\n");
}

module_init(ptree_init);
module_exit(ptree_exit);
