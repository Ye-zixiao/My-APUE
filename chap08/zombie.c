#include"MyAPUE.h"
#include<sys/wait.h>

int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		printf("output from child(pid: %d)\n", getpid());
	}
	else {
		sleep(2);
		if (system("ps aux|grep defunct") < 0)
			err_sys("system error\n");
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("watipid error\n");
	}

	exit(EXIT_SUCCESS);
}