#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#define __NR_print_group 329

/*
Typedef function pointers to syscalls so that declaring
orig_open_t orig_open 
is the same as having written
asmlinkage int (*orig_open)(const char __user *, int, umode_t);

See /include/linux/syscalls.h for the function headers
*/





typedef asmlinkage int (*orig_print_group_t)(int); 
orig_print_group_t orig_print_group;


unsigned long cr0; //Stores the cr0 register which maintains read/write access
unsigned long *sys_call_table;

unsigned long *get_syscall_table(void)
{
	unsigned long *syscall_table;
	unsigned long int i;

	for (i = PAGE_OFFSET; i < ULONG_MAX; i += sizeof(void *)) {
		syscall_table = (unsigned long *)i;
	//sys_close seems to be one of the only syscalls still exported as a kernel symbol
		if (syscall_table[__NR_close] == (unsigned long)sys_close) 
			return syscall_table;
	}
	return NULL;
}



asmlinkage int hacked_print_group(int location){
	//This is a re-write of our syscall, and what we will replace our system call with.
	if(location == 0){
		printk(KERN_CRIT "rreK naI ,volzoK erdnaxelA");
	}
	else{
		printk(KERN_INFO "!kciiiiiiiiiir yniT");
	}
	return 0;

}


static int __init hook_init(void)
{
	printk("Starting module\n");

	sys_call_table = get_syscall_table();//grabbing current syscall table 
	if (!sys_call_table)
		return -1;

	cr0 = read_cr0(); //Store the original cr0 flags

	//Save the old syscalls
	orig_print_group = (orig_print_group_t)sys_call_table[__NR_print_group];



	printk("Hooking print_group\n");
	write_cr0(cr0 & ~0x00010000); //Unprotect memory (Kernel now can't tell what's read-only! Oh noes!)
	sys_call_table[__NR_print_group] = (unsigned long)hacked_print_group; //Implant our hooks
	write_cr0(cr0); //Reprotect memory (The kernel is now happy again)

	if(sys_call_table[__NR_print_group] == (unsigned long)hacked_print_group)//error check to make sure it worked
	printk("print_group hooked\n");
	else{
		printk("Failed to hook print_group\n");
		return -1;
	}
	return 0;
}

static void __exit hook_cleanup(void)
{
	printk("Unhooking print_group\n");
	write_cr0(cr0 & ~0x00010000);
	sys_call_table[__NR_print_group] = (unsigned long)orig_print_group; //Remove our hook
	write_cr0(cr0);
	printk("print_group unhooked\n");
}

module_init(hook_init);
module_exit(hook_cleanup);

MODULE_LICENSE("About tree-fiddy");
MODULE_AUTHOR("Ian Kerr and Alex Kozlov");
MODULE_DESCRIPTION("Hooking our own syscall");
