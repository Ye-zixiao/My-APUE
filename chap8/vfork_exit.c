#include"MyAPUE.h"

int global_var = 6;

int main(void)
{
	int var = 88;
	pid_t pid;

	printf("before vfork...\n");
	if ((pid = vfork()) < 0)
		err_sys("vfork error\n");
	else if (pid == 0) {
		global_var++;
		var++;

		//1、并没有关闭父进程的I/O流
		//exit(0);

		//2、强制关闭父进程的I/O流
		if (fclose(stdout) != 0)
			err_sys("fclose error\n");
		_exit(0);
	}
	
	if (printf("pid:%d, ppid:%d, var=%d, global_var:%d\n",
		getpid(), getppid(), var, global_var) == -1)
		fputs("stdout closed\n", stderr);

	exit(0);
}