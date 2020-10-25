//#include"apue.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

int globalvalue = 6;

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(void)
{
	/*若开启了-O优化选项，需要将在这前面加入volatile以告诉编译器不要优化这个变量；
		当然你也可以不选择优化。但老子就是不想改makefile！😋*/
	volatile int var = 88;
	pid_t pid;

	printf("before fork...\n");
	if ((pid = vfork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		++globalvalue;
		++var;
		_exit(0);
		//exit(0);
	}
	
	printf("pid = %ld, globalvalue = %d, var = %d\n",
		(long)getpid(), globalvalue, var);
	exit(EXIT_SUCCESS);
}