#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
	int fd = open("testfile", O_CREAT);
	if(fd == -1){
		printf("File failed to open\n");
	}
	else{
		printf("File opened successfully");
	}
	close(fd);
	return 0;
}
