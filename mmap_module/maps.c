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
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amit Shani");
MODULE_DESCRIPTION("the extended /proc/self/maps");
MODULE_VERSION("0.01");

extern void show_map_vma(struct seq_file *m, struct vm_area_struct *vma);

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
		if (is_stack(mmap)) {
			struct seq_file* f = kmalloc(sizeof(struct seq_file), GFP_KERNEL);
			char * buf = kmalloc(4096, GFP_KERNEL);
			f->buf = buf;
			f->size = 4096;
			show_map_vma(f, mmap);
			printk(KERN_INFO "%s", f->buf);
			kfree(buf);
			kfree(f);
		}
		printk(KERN_INFO "vm_start: %lu, is_stack: %s\n", mmap->vm_start, is_stack(mmap) ? "true":"false");
		mmap = mmap->vm_next;
	}
}

static int page_flag_range(pte_t *pte, unsigned long addr, unsigned long end, struct mm_walk *walk) {
	struct seq_file * f = walk->private;
	struct page * p;
	int ref_count;
	if (!pte_present(*pte)) {
		seq_putc(f, '.');
		return 0;
	}
	// p = vm_normal_page(walk->vma, addr, pte);
	// if (p == NULL) {
	// 	printk(KERN_INFO "no page?");
	// 	return 0;
	// }
	pfn = pte_pfn(*pte);
	if (!pfn_valid(pfn)) {
		printk(KERN_INFO "pfn invalid");
		return 0;
	}

	p = pfn_to_page(pfn);
	if (p == NULL) {
		printk(KERN_INFO "no page?");
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

const int SEQ_READ_SIZE = 8192;
static const struct mm_walk_ops page_flags_walk_ops = {
	.pte_entry = page_flag_range,
};

void print_extended_vma(struct task_struct * task) {
	struct vm_area_struct *vma;
	struct mm_struct *mm = task->mm;
	
	struct seq_file * f = kmalloc(sizeof(struct seq_file), GFP_KERNEL);
	char * buf = kmalloc(SEQ_READ_SIZE, GFP_KERNEL);
	f->buf = buf;
	f->size = SEQ_READ_SIZE;

	down_read(&mm->mmap_sem);
	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		show_map_vma(f, vma);
		f->count--; // remove newline char
		walk_page_vma(vma, &page_flags_walk_ops, (void *)f);
		seq_putc(f, '\n');
	}
	up_read(&mm->mmap_sem);

	printk(KERN_INFO "%s", f->buf);
	
	kfree(buf);
	kfree(f);
}

static int __init maps_init(void)
{
	printk(KERN_INFO "Hello, World!\n");
	// print_shit(get_task_struct_of_pid(6626));
	print_extended_vma(get_task_struct_of_pid(6626));
	return 0;
}

static void __exit maps_exit(void)
{
	printk(KERN_INFO "Goodbye, World!\n");
}

module_init(maps_init);
module_exit(maps_exit);
