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
MODULE_DESCRIPTION("the extended /proc/self/maps");
MODULE_VERSION("0.01");

struct task_struct* get_self_task_struct(void) {
	struct task_struct* ret;
	pid_t pid = task_pid_nr(current);
	rcu_read_lock();
	ret = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();
	return ret;
}

static int __init maps_init(void)
{
	printk(KERN_INFO "Hello, World!\n");

	return 0;
}

static void __exit maps_exit(void)
{
	printk(KERN_INFO "Goodbye, World!\n");
}

module_init(maps_init);
module_exit(maps_exit);
