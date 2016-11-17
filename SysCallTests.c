#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#define __NR_print_group 329
#define __NR_find_process 330
static inline long mysys_print_group(int in) { return syscall(__NR_print_group, in); }
static inline long mysys_find_process(char *process) { return syscall(__NR_find_process, process); }

int main(){
	char *argv[] = {"/bin/ps", "aux", 0};
	char *envp[] = {"/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games"};
	printf("Plz\n");
	mysys_print_group(0);
	printf("Plz plz\n");
	mysys_print_group(1);
	printf("Looking for systemd...\n");
	mysys_find_process("systemd");
	execve(argv[0], &argv[0], envp);//calling system call directly. 
	//If done in tandem with hook module, a message should pop up and the process with PID of 1 should be invisible.
	
	//system("touch test.txt");
	return 0;//Alex: Note to self-this ends the program, no matter what.
	
	
	


}
