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

struct task_struct* get_self_task_struct(void) {
	struct task_struct* ret;
	pid_t pid = task_pid_nr(current);
	rcu_read_lock();
	ret = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();
	return ret;
}

static int page_flag_range(pte_t *pte, unsigned long addr, unsigned long end, struct mm_walk *walk) {
	struct seq_file * f = walk->private;
	struct page * p;
	int ref_count;
	long unsigned int pfn;
	if (!pte_present(*pte)) {
		seq_putc(f, '.');
		return 0;
	}
	pfn = pte_pfn(*pte);
	if (!pfn_valid(pfn)) {
		printk(KERN_INFO "pfn invalid\n");
		return 0;
	}

	p = pfn_to_page(pfn);
	if (p == NULL) {
		printk(KERN_INFO "no page?\n");
		return 0;
	}
	ref_count = page_ref_count(p);
	if (ref_count < 10) {
		seq_putc(f, 48+ref_count);
	} else {
		seq_putc(f, 'x');
	}
	return 0;
}

static const struct mm_walk_ops page_flags_walk_ops = {
	.pte_entry = page_flag_range,
};

size_t print_extended_vma(struct task_struct * task, char __user *buf, size_t size) {
	struct vm_area_struct *vma;
	struct mm_struct *mm = task->mm;
	
	struct seq_file * f = kmalloc(sizeof(struct seq_file), GFP_KERNEL);
	char * buf = kmalloc(size, GFP_KERNEL);
	f->buf = buf;
	f->size = size;

	down_read(&mm->mmap_sem);
	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		show_map_vma(f, vma);
    if (f->count > 0 && f->count < size) // make sure we are in bounds and we don't override the last char
		  f->buf[f->count-1] = ' '; // replace newline with whitespace
		walk_page_vma(vma, &page_flags_walk_ops, (void *)f);
		seq_putc(f, '\n');
	}
	up_read(&mm->mmap_sem);

  copy_to_user(buf, f->buf, f->count);
	
	kfree(buf);
	kfree(f);
  return f->count;
}


asmlinkage int mapspages(unsigned long start, unsigned long end, char __user *buf, size_t size)
{
  if (start > end) {
    return -EINVAL;
  }
  if (!access_ok(buf, size)) {
    return -EFAULT;
  }
  struct task_struct t = get_self_task_struct();
  return print_extended_vma(t, buf, size);
}