#include"MyAPUE.h"

void func(void) {
	pid_t pid;
	volatile int local_value = 10;//强制要求函数局部变量分配在过程帧中`

	if ((pid = vfork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		printf("value= %d\n", local_value);
		printf("pid: %d,ppid: %d, child in func\n", getpid(), getppid());
	}
	else {
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error\n");
		printf("value= %d\n", local_value);
		printf("pid: %d,ppid: %d, parent in func\n", getpid(), getppid());
	}
	return;/*子进程从在父进程的地址空间中返回了func并很快结束了main函数，
			 这会导致父进程的过程帧不复存在*/
}

int main(void)
{
	printf("pid: %d,ppid: %d,parent before func executed\n\n",
		getpid(), getppid());
	func();
	printf("pid: %d,ppid: %d,child or parent??\n",
		getpid(), getppid());
	
	exit(EXIT_SUCCESS);
}
