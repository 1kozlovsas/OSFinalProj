#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

struct timeval t;

static int __init hook_init(void)
{
	
	do_gettimeofday(&t);//using struct timeval's methods to obtain time
	printk("Starting module at epoch time %lu\n", t.tv_sec);//outputting in demsg
	return 0;
}

static void __exit hook_cleanup(void)
{
	do_gettimeofday(&t);
	printk("Closing module at epoch time %lu\n", t.tv_sec);
}

module_init(hook_init);
module_exit(hook_cleanup);

MODULE_LICENSE("About tree-fiddy");
MODULE_AUTHOR("Ian Kerr and Alex Kozlov");
MODULE_DESCRIPTION("Printing a message to the kernel log");
