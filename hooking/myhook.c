#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/syscalls.h>


#include <linux/sched.h>


#include <linux/dirent.h>
#include <linux/slab.h>
#include <linux/version.h> 
	#include <linux/proc_ns.h>


	#include <linux/fdtable.h>

#define MODULE_NAME "Simple syscall hook"
/*
Typedef function pointers to syscalls so that declaring
orig_open_t orig_open 
is the same as having written
asmlinkage int (*orig_open)(struct tty_struct *, struct file *);
*/
typedef asmlinkage int (*orig_open_t)(struct tty_struct *, struct file *); 

orig_open_t orig_open;
//asmlinkage int (*orig_open)(struct tty_struct *, struct file *); 

unsigned long cr0; //Stores the cr0 register which maintains read/write access
unsigned long *sys_call_table;

unsigned long *get_syscall_table(void)
{
	unsigned long *syscall_table;
	unsigned long int i;

	for (i = PAGE_OFFSET; i < ULONG_MAX; i += sizeof(void *)) {
		syscall_table = (unsigned long *)i;
		if (syscall_table[__NR_close] == (unsigned long)sys_close) //sys_close seems to be one of the only syscalls still exported as a kernel symbol
			return syscall_table;
	}
	return NULL;
}

asmlinkage int hacked_open(struct tty_struct *tty, struct file *fp){
	printk("Your open is nopen!");
	return orig_open(tty, fp);
}

static int __init hook_init(void)
{
	sys_call_table = get_syscall_table();
	if (!sys_call_table)
		return -1;

	cr0 = read_cr0();

	orig_open = (orig_open_t)sys_call_table[__NR_open];
	//orig_open = (asmlinkage int (*)(struct tty_struct *, struct file *))sys_call_table[__NR_open];

	write_cr0(cr0 & ~0x00010000); //Unprotect memory (Kernel now can't tell what's read-only! Oh noes!)
	sys_call_table[__NR_open] = (unsigned long)hacked_open; //Implant our hook
	write_cr0(cr0); //Reprotect memory (The kernel is now happy again)

	return 0;
}

static void __exit hook_cleanup(void)
{
	write_cr0(cr0 & ~0x00010000);
	sys_call_table[__NR_open] = (unsigned long)orig_open; //Remove our hook
	write_cr0(cr0);
}

module_init(hook_init);
module_exit(hook_cleanup);

MODULE_LICENSE("About tree-fiddy");
MODULE_AUTHOR("Ian Kerr and Alex Kozlov");
MODULE_DESCRIPTION("Basic Syscall Hooking Attempt");
