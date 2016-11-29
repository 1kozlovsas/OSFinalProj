#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char* envp[]){
	execve("/usr/bin/ls", (char*[]){"ls", "/", NULL}, envp);
	return 0;
}
