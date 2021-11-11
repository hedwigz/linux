#include <linux/prinfo.h>
#include <asm/errno.h>

typedef int (*ptree_func)(struct prinfo *buf, int *nr, int pid);
ptree_func ptree_impl;
int register_ptree(ptree_func func)
{
	ptree_impl = func;
}

void unregister_ptree(ptree_func func)
{
	ptree_impl = NULL;
}

asmlinkage int sys_ptree(struct prinfo *buf, int *nr, int pid)
{
	if (ptree_impl != NULL) {
		return module_impl(buf);
	} else {
		return -ENOSYS;
	}
	return 0;
}