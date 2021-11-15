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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amit Shani");
MODULE_DESCRIPTION("Learning how to build linux modules");
MODULE_VERSION("0.02");

struct pid_level {
	int level;
	pid_t pid;
};

extern struct task_struct init_task;

void to_prinfo(struct task_struct *task, struct prinfo *pinfo, int level)
{
	memcpy(pinfo->comm, task->comm, 16);
	pinfo->state = task->state;
	pinfo->pid = task->pid;
	pinfo->level = level;
	if (task->cred) {
		pinfo->uid = task->cred->uid.val;
	}
}

struct task_struct* get_p(int pid)
{
	if (pid == 0) {
		return &init_task;
	}
	// we need to lock here, as documented here:
	// https://github.com/torvalds/linux/blob/5d867ab037e58da3356b95bd1a7cb8efe3501958/include/linux/pid.h#L106
	rcu_read_lock();
	struct task_struct* ret = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();
	return ret;
}

int get_childs(pid_t root_pid, struct pid_level * pids, struct prinfo * buf, int n, int level) {
  int i = 0;
  struct task_struct *p;
  rcu_read_lock();
  list_for_each_entry_rcu(p, &init_task.tasks, tasks) {
    if (i >= n) {
      break;
    }
    // check if process has parent
    if (!p->parent) {
      continue;
    }
    // is parent our root_pid?
    if (p->parent->pid == root_pid) {
			struct prinfo pinfo = { 0 };
			to_prinfo(p, &pinfo, level);
			printk(KERN_INFO "found child: %d: %s\n", pinfo.pid, pinfo.comm);
			memcpy(&buf[i], &pinfo, sizeof(struct prinfo));
			// copy_to_user(&buf[i], &pinfo, sizeof(struct prinfo));
			// printk(KERN_INFO "actual pinfo on memory: %d: %s\n.", buf[i].pid, buf[i].comm);
			pids[i].pid = p->pid;
			pids[i].level = level;
      ++i;
      continue;
    }
  }
  rcu_read_unlock();
	printk(KERN_INFO "done looking for childs of: %d\n", root_pid);
  return i;
}

int ptree(struct prinfo *buf, int *nr, int pid)
{
	printk(KERN_INFO "entering ptree...\n");
  // syscall already checked that nr > 0
	int n = *nr;
	int got = 0;
  int i = 0;
  int root_idx = 0;
	int level = 0;

	// allocate array of PIDs
	struct pid_level *pids;
	pids = kmalloc_array(n, sizeof(struct pid_level), GFP_KERNEL);
	printk(KERN_INFO "allocated array for pids\n");
	if (!pids)
		return -ENOMEM;
  pids[0].pid = pid;

	// insert init_task
	struct prinfo pinfo = { 0 };
	to_prinfo(&init_task, &pinfo, 0);
	memcpy(buf, &pinfo, sizeof(struct prinfo));

	do {
    struct pid_level root_pid = pids[root_idx];
    printk(KERN_INFO "Looking for child processes of pid: %d\n", root_pid.pid);
    ++root_idx;
    i += get_childs(root_pid.pid, pids, &buf[i], n-i, root_pid.level+1);
	} while (root_idx < i && i < n);
	got = i;
  printk(KERN_INFO "got total of %d processes\n", got);
	
	kfree(pids);
  return got;
}

static int __init ptree_init(void)
{
  int reg_err;
	printk(KERN_INFO "Hello, World!\n");
	reg_err = register_ptree(&ptree);
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
