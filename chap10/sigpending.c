#include"MyAPUE.h"
#include<signal.h>
#include<errno.h>

static void quit_handler(int sig);

int main(void)
{
	sigset_t newmask, oldmask, pendmask;

	if (signal(SIGQUIT, quit_handler) == SIG_ERR)
		err_sys("signal error\n");

	/*
		屏蔽SIGQUIT信号，若屏蔽之后，在这期间即使发送了多个SIGQUIT信号，
		在开放对SIGQUIT的接收后也只会提交一次SIGQUIT信号，而不是排队
	*/
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) == -1)
		err_sys("sigprocmask error\n");
	sleep(5);

	/*
		检查SIGQUIT信号是否被屏蔽
	*/
	if (sigpending(&pendmask) == -1)
		err_sys("sigpending error\n");
	if (sigismember(&pendmask, SIGQUIT))
		printf("\nSIGQUIT pending\n");

	/*
		恢复到原来的信号屏蔽字状态
	*/
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) == -1)
		err_sys("sigprocmask error\n");
	printf("SIGQUIT unblocked\n");
	sleep(5);
	exit(0);
}

static void quit_handler(int sig) {
	printf("SIGQUIT is received\n");
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		err_sys("signal error\n");
}