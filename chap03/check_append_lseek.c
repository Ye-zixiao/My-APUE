#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#define BUFSIZE 4096

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(-1);
}

int main(int argc,char*argv[])
{
	if (argc != 2)
		err_sys("Usage:command filename\n");

	int fd;
	if ((fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT)) < 0)
		err_sys("open error\n");
	if (write(fd, "show me the code\n", 17) != 17)
		err_sys("append write error\n");

	if (lseek(fd, 0, SEEK_SET) < 0)
		err_sys("lseek error\n");
	if (write(fd, "write from start\n", 17) != 17)
		err_sys("write error\n");

	close(fd);
	exit(0);
}