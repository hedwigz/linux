#include <linux/prinfo.h>
#include <linux/uaccess.h>
#include <asm/errno.h>
#include <linux/kmod.h>
#include <linux/ptree.h>

DEFINE_SPINLOCK(register_lock);

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
EXPORT_SYMBOL_GPL(register_ptree);

void unregister_ptree(ptree_func func)
{
  spin_lock(&register_lock);
	ptree_impl = NULL;
  spin_unlock(&register_lock);
}
EXPORT_SYMBOL_GPL(unregister_ptree);

asmlinkage int sys_ptree(struct prinfo *buf, int *nr, int pid)
{
  if (!access_ok(nr, sizeof(int))) {
    return -EFAULT;
  }
  if (*nr < 1) {
    return -EINVAL;
  }
  if (!access_ok(buf, (*nr)*sizeof(struct prinfo))) {
    return -EFAULT;
  }
  spin_lock(&register_lock);
  if (ptree_impl == NULL) {
    request_module("ptree");
  }
  if (ptree_impl == NULL) {
    spin_unlock(&register_lock);
    return -ENOSYS;
  }
  // struct prinfo tmp;
  int ret = ptree_impl(buf, nr, pid);
  spin_unlock(&register_lock);
  // copy_to_user(buf, &tmp, sizeof(tmp));
  return 0;
}