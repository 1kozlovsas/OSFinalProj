#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#define __NR_print_group 329
static inline long mysys_print_group(int in) { return syscall(__NR_print_group, in); }

int main(){
	char *argv[] = {"/usr/bin/touch", "test.txt", 0};
	char *envp[] = {"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games",NULL};
	printf("Plz\n");
	mysys_print_group(0);
	printf("Plz plz\n");
	mysys_print_group(1);
	execve(argv[0], &argv[0], envp);//calling system call directly. If done in tandem with hook module, "that tickles! should pop up".
	//system("touch test.txt");
	return 0;//Alex: Note to self-this ends the program, no matter what.
	
	
	


}
