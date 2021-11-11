#include <linux/prinfo.h>
#include <asm/errno.h>

​typedef int (*ptree_func)(struct prinfo *buf, int *nr, int pid);

ptree_func* module_impl;

​int register_ptree(ptree_func func) {
  module_impl = func;
}
​void unregister_ptree(ptree_func func) {
  module_impl = NULL;
}

asmlinkage int sys_ptree (struct *prinfo buf , int *nr, int pid) {
  if (module_impl) {
    return module_impl(buf)
  } else {
    return -ENOSYS;
  }
  return 0;
}