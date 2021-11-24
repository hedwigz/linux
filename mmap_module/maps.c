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


struct task_struct* get_task_struct_of_pid(pid_t pid) {
	struct task_struct* ret;
	rcu_read_lock();
	ret = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();
	return ret;
} 

// src: task_mmu.c:268
static int is_stack(struct vm_area_struct *vma)
{
	return vma->vm_start <= vma->vm_mm->start_stack &&
		vma->vm_end >= vma->vm_mm->start_stack;
}

void print_shit(struct task_struct * task) {
	struct vm_area_struct *mmap = task->mm->mmap;
	while (mmap != NULL) {
		printk(KERN_INFO "vm_start: %lu, is_stack: %s\n", mmap->vm_start, is_stack(mmap) ? "true":"false");
		mmap = mmap->vm_next;
	}
}

static int __init maps_init(void)
{
	printk(KERN_INFO "Hello, World!\n");
	print_shit(get_task_struct_of_pid(6626));
	return 0;
}

static void __exit maps_exit(void)
{
	printk(KERN_INFO "Goodbye, World!\n");
}

module_init(maps_init);
module_exit(maps_exit);
