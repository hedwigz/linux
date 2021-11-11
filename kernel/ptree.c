#include <linux/prinfo.h>
#include <asm/errno.h>

// DEFINE_SPINLOCK(register_lock);

typedef int (*ptree_func)(struct prinfo *buf, int *nr, int pid);
ptree_func ptree_impl;
int register_ptree(ptree_func func)
{
  // spin_lock(&register_lock);
	ptree_impl = func;
  // spin_unlock(&register_lock);
}

void unregister_ptree(ptree_func func)
{
	ptree_impl = NULL;
}

asmlinkage int sys_ptree(struct prinfo *buf, int *nr, int pid)
{
	if (ptree_impl != NULL) {
		return ptree_impl(buf);
	} else {
		return -ENOSYS;
	}
	return 0;
}