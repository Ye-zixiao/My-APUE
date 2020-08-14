#include"../include/MyAPUE.h"
#include<termios.h>

static void pr_id(const char* info) {
	pid_t pid;

	printf("From %s Process: \n", info);
	printf("Parent process id: %d\n", getppid());
	printf("This  process  id: %d\n", getpid());
	printf("Process group  id: %d\n", getpgrp());
	printf("Session leader id: %d\n", getsid(0));
	if ((pid = tcgetsid(STDOUT_FILENO)) == -1) {
		if(ENOTTY==errno)
			printf("This process has no controlling terminal\n");
	}
	else {
		printf("Controlling process id: %d\n",
			pid);
	}
}

int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		if (setsid() == -1)//创建新会话以及新进程组
			err_sys("setsid error\n");
		pr_id("Child");
	}
	else {
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error\n");
		printf("---------------------------------------\n");
		pr_id("Parent");
	}
	
	exit(EXIT_SUCCESS);
}
