#include<linux/kernel.h>
#include<linux/linkage.h>

asmlinkage void _print_kernel(char *dmesg){
	printk(dmesg);
	return; 
}
