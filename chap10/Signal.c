#include"MyAPUE.h"
#include<signal.h>
#include<setjmp.h>
#include<time.h>
#include<sys/time.h>

typedef void Sig_handler(int);
static jmp_buf env_buf;

Sig_handler* Signal(int signo, Sig_handler* func);
static void alrm_handler(int sig);
void print_now(void);


int main(void)
{
	if (Signal(SIGALRM, alrm_handler) == SIG_ERR)
		err_sys("Signal error\n");

	print_now();
	if (setjmp(env_buf) == 0) {
		alarm(5);
		pause();
	}
	print_now();

	if (setjmp(env_buf) == 0) {
		alarm(1);
		pr_mask("second alarm(): ");
		pause();/*由于上一次longjmp函数跳转回main后，并没有恢复原来的信号屏蔽字，
				故此时进程再接收到SIGALRM信号一定会被屏蔽而处于未决状态，这样再
				调用pause函数会进入长期休眠的状态，除非收到SIGALRM以外的信号*/
	}
	
	exit(0);
}


/*===================================================*
*			 function defination
* ===================================================*
*/
Sig_handler*
Signal(int signo, Sig_handler* sig_handler) {
	struct sigaction act, old_act;

	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		/*
		 *	尽可能使由这些信号（除了SIGALRM）导致中断的系统调用能够
		 *	自动重启动
		 * */
		act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else {
		act.sa_flags |= SA_RESTART;
	}
	if (sigaction(signo, &act, &old_act) == -1)
		err_sys("sigaction error\n");
	return old_act.sa_handler;
}

static void alrm_handler(int sig) {
	if (sig == SIGALRM)
		printf("received signal SIGALRM %d\n",
			sig);
	longjmp(env_buf, 1);

}

void print_now(void) {
	const int BufSize = 64;
	struct timeval timevalbuf;
	struct tm* ptm;
	char buf[BufSize];

	gettimeofday(&timevalbuf, NULL);
	if ((ptm = localtime(&timevalbuf.tv_sec)) == NULL)
		err_sys("localtime error\n");
	if (strftime(buf, BufSize, "%Y-%m-%d %H:%M:%S", ptm) == 0)
		err_sys("strftime error\n");
	printf("Current time is %s\n", buf);
}
