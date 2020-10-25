#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

#define BUFSIZE 4096

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	if (argc != 2)err_sys("usage:myreadlnk [linkname]\n");
	
	int val;
	char buf[BUFSIZE];
	if ((val = readlink(argv[1], buf, BUFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, val) != val)
			err_sys("write error");
	if (val < 0)err_sys("readlink error\n");
	exit(EXIT_SUCCESS);
}