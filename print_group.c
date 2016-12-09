#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#define __NR_print_group 329
static inline long mysys_print_group(int in) { return syscall(__NR_print_group, in); }

int main(){
//Just calling our custom system call, changing parameter values to print to syslog and dmesg.	
	
	printf("Plz\n");
	mysys_print_group(0);
	printf("Plz plz\n");
	mysys_print_group(1);
	return 0;
}
