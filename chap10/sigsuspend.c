#include "MyAPUE.h"

static void sig_int(int signo) {
	pr_mask("\nin sig_int(): ");
}


int main(void)
{
	sigset_t newmask, oldmask, waitmask;

	pr_mask("program start: ");
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error\n");
	sigemptyset(&newmask);
	sigemptyset(&oldmask);
	sigaddset(&newmask, SIGINT);
	sigaddset(&waitmask, SIGUSR1);

	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) == -1)	//先屏蔽SIGINT信号
		err_sys("sigprocmask error\n");
	/*
		critical region
	*/
	int k = 0;
	for (volatile int i = 0; i < 400000; i++)
		for (volatile int j = 0; j < 5000; j++)
			k = i * j;

	pr_mask("in critical region: ");
	
	if (sigsuspend(&waitmask) != -1)						/*暂时不屏蔽SIGINT信号，而是只屏蔽SIGUSR1信号，
															在该阶段可以处理进入临界区时收到的SIGINT信号*/
		err_sys("sigsuspend error\n");
	pr_mask("after return from sigsuspend: ");

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) == -1)		//恢复到原来的信号屏蔽字状态
		err_sys("sigprocmask error\n");
	pr_mask("program exit: ");
	exit(0);
}
