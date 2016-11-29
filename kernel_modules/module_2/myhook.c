#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/kthread.h>


/*int process_finder(void *input){
	int highestpid; 
	highestpid = 0;
	char *process_name = "my_proc"; //Look for this process name
	for(;;){
	struct task_struct *g;
		for_each_process(g) { //Loop through all the processes
			if(strcmp(process_name, g->comm) == 0){ //Find the processes which match the process_name
				if(g->pid > highestpid){
					printk("%s has been started with pid %d\n", process_name, g->pid);
					highestpid = g->pid;			
				}
			}
	    	}
	msleep(1000);	
	}
}*/


static int __init hook_init(void)
{
	printk("Starting module\n");
	//kthread_create(&process_finder,(void *)"Process finder","Process finder");
	int highestpid; 
	highestpid = 0;
	char *process_name = "my_proc"; //Look for this process name
	for(;;){
	struct task_struct *g;
		for_each_process(g) { //Loop through all the processes
			if(strcmp(process_name, g->comm) == 0){ //Find the processes which match the process_name
				if(g->pid > highestpid){
					printk("%s has been started with pid %d\n", process_name, g->pid);
					highestpid = g->pid;			
				}
			}
	    	}
	msleep(1000);	
	}
	return 0;
}

static void __exit hook_cleanup(void)
{
	printk("Closing module\n");
}

module_init(hook_init);
module_exit(hook_cleanup);

MODULE_LICENSE("About tree-fiddy");
MODULE_AUTHOR("Ian Kerr and Alex Kozlov");
MODULE_DESCRIPTION("Monitoring a process");
