#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
	if (argc != 3)
		err_sys("usage:myrename [oldname] [newname]\n");
	if (rename(argv[1], argv[2]) < 0)
		err_sys("rename error\n");
	exit(EXIT_SUCCESS);
}