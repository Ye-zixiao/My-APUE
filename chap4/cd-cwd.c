#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define __NAME_MAX 256

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(int argc,char*argv[])
{
	char* target_dir = "/home/yexinhai";
	if (argc == 2)
		target_dir = argv[1];

	char* ptr = (char*)malloc(__NAME_MAX);
	if (chdir(target_dir) < 0)
		err_sys("chdir failed\n");
	if (getcwd(ptr, __NAME_MAX) == NULL)
		err_sys("getcwd failed\n");
	printf("cwd=%s\n", ptr);

	exit(EXIT_SUCCESS);
}