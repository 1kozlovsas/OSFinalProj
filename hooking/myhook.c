#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/slab.h> //For kzalloc
#define MODULE_NAME "Simple syscall hook"
/*
Typedef function pointers to syscalls so that declaring
orig_open_t orig_open 
is the same as having written
asmlinkage int (*orig_open)(const char __user *, int, umode_t);

See /include/linux/syscalls.h for the function headers
*/


typedef asmlinkage int (*orig_execve_t)(const char __user *, const char __user *const __user *, const char __user *const __user *); 
orig_execve_t orig_execve;

/*typedef asmlinkage int (*orig_open_t)(const char __user *, int, umode_t); 
orig_open_t orig_open;*/

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



/*asmlinkage int hacked_open(const char __user *filename, int flags, umode_t mode){
	//if(strcmp(filename, "testfile") == 0){
		printk("Now opening testfile");
	//}
	//printk("Now opening the file %s\n", filename);
	printk("potatoes");
	return orig_open(filename, flags, mode);
}*/

asmlinkage int hacked_execve(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp){

	int err;
	char *kfilename;
	kfilename = kzalloc(30,GFP_KERNEL);
	err = copy_from_user(kfilename, filename, 30);
	if(!err){
		if(strcmp(kfilename, "/usr/bin/touch") == 0)
		printk("That tickles!");
	}
	return orig_execve(filename, argv, envp);
}

static int __init hook_init(void)
{
	printk("Starting module\n");

	sys_call_table = get_syscall_table();
	if (!sys_call_table)
		return -1;

	cr0 = read_cr0();

	orig_execve = (orig_execve_t)sys_call_table[__NR_execve];
	//orig_open = (orig_open_t)sys_call_table[__NR_open];


	printk("Hooking execve\n");
	write_cr0(cr0 & ~0x00010000); //Unprotect memory (Kernel now can't tell what's read-only! Oh noes!)

	//sys_call_table[__NR_open] = (unsigned long)hacked_open; //Implant our hook
	sys_call_table[__NR_execve] = (unsigned long)hacked_execve;	

	write_cr0(cr0); //Reprotect memory (The kernel is now happy again)
	if(sys_call_table[__NR_execve] == (unsigned long)hacked_execve)
	printk("execve hooked\n");
	else{
		printk("Failed to hook execve\n");
		return -1;
	}
	return 0;
}

static void __exit hook_cleanup(void)
{
	printk("Unhooking execve\n");
	write_cr0(cr0 & ~0x00010000);
	//sys_call_table[__NR_open] = (unsigned long)orig_open; //Remove our hook
	sys_call_table[__NR_execve] = (unsigned long)orig_execve;	
	write_cr0(cr0);
	printk("execve unhooked\n");
}

module_init(hook_init);
module_exit(hook_cleanup);

MODULE_LICENSE("About tree-fiddy");
MODULE_AUTHOR("Ian Kerr and Alex Kozlov");
MODULE_DESCRIPTION("Basic Syscall Hooking Attempt");
