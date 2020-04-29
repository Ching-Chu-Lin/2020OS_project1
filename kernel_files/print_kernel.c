#include<linux/kernel.h>
#include<linux/linkage.h>

asmlinkage void sys_print_kernel(char *dmesg){
	printk(dmesg);
	return; 
}
