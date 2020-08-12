#include"../include/MyAPUE.h"

static void sig_int(int signo) {
	printf("caught SIGINT\n");
}

static void sig_chld(int signo) {
	printf("caught SIGCHLD\n");
	printf("wait(NULL) return: %d\n",
		wait(NULL));
	/*
		POSIX.1规定wait若取得了子进程的终止状态，则之前阻塞的SIGCHLD信号
		不再像当前进程递交。但是这个规定在Linux上并没有实现。
			所以当此时调用wait会返回-1，因为此时并没有子进程可以获得终止状态
	*/
}

int main(void)
{
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error\n");
	if (signal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("signal error\n");
	if (mysystem("/bin/ed") < 0)
		err_sys("System() error\n");
	exit(0);
}
