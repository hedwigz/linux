#include <linux/prinfo.h>
#include <asm/errno.h>
#include <linux/kmod.h>

DEFINE_SPINLOCK(register_lock);

typedef int (*ptree_func)(struct prinfo *buf, int *nr, int pid);
ptree_func ptree_impl;
int register_ptree(ptree_func func)
{
  spin_lock(&register_lock);
  if (ptree_impl == NULL) {
    ptree_impl = func;
    spin_unlock(&register_lock);
    return 0;
  }
  spin_unlock(&register_lock);
  return -EBUSY;
}

void unregister_ptree(ptree_func func)
{
  spin_lock(&register_lock);
	ptree_impl = NULL;
  spin_unlock(&register_lock);
}

asmlinkage int sys_ptree(struct prinfo *buf, int *nr, int pid)
{
  spin_lock(&register_lock);
  if (ptree_impl == NULL) {
    request_module("ptree");
  }
  if (ptree_impl == NULL) {
    spin_unlock(&register_lock);
    return -ENOSYS;
  }
  spin_unlock(&register_lock);
  // TODO: handle memory copy between userspace and kernel space 
  return ptree_impl(buf, nr, pid);
}