#include"../include/MyAPUE.h"

int main(void)
{
	pid_t pid;

	TELL_WAIT();
	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		printf("hello world from child(pid: %d)\n",
			getpid());
		TELL_PARENT(getppid());
	}
	else {
		WAIT_CHILD();
		printf("hello world from parent(pid: %d)\n",
			getpid());
	}

	exit(0);
}
