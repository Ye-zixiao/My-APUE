#include"MyAPUE.h"
#include<sys/wait.h>

int main(void)
{
	pid_t pid;
	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		if ((pid = fork()) < 0)
			err_sys("fork error\n");
		else if (pid > 0)//子进程终止，将孙子进程过继给init进程😂
			exit(EXIT_SUCCESS);

		sleep(2);
		printf("second child, parent pid = %ld\n", (long)getppid());
		exit(EXIT_SUCCESS);
	}

	if (waitpid(pid, NULL, 0) != pid)
		err_sys("waitpid error\n");
	exit(EXIT_SUCCESS);
}