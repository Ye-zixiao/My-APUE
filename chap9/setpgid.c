#include"MyAPUE.h"
#include<sys/wait.h>


int main(int argc, char* argv[])
{
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		if (setpgid(getpid(), getppid()) == -1)
			err_sys("setpgid error\n");
		execlp("./getpgid", "getpgid", (char*)0);

		exit(EXIT_FAILURE);
	}
	else {
		if (setpgid(pid, getpid()) == -1)
			err_sys("setpgid error\n");
		if (waitpid(-getpid(), NULL, 0) != pid)//等某一个进程组中的任一终止进程
			err_sys("waitpid error\n");
		execlp("./getpgid", "getpgid", (char*)0);
		exit(EXIT_FAILURE);
	}
}