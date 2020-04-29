#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/ktime.h>

asmlinkage void sys_my_time(struct timespec *t){
  getnstimeofday(t);
  return;
}
