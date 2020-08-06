﻿#include"MyAPUE.h"
#include<sys/wait.h>

int main(void)
{
	pid_t pid;
	int status;

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0)
		exit(7);

	if (wait(&status) != pid)
		err_sys("wait error\n");
	pr_exit(status);

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0)
		abort();

	if (wait(&status) != pid)
		err_sys("wait error\n");
	pr_exit(status);

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0)
		status /= 0;

	if (wait(&status) != pid)
		err_sys("wait error\n");
	pr_exit(status);
	
	exit(0);
}