/*
 *	在Linux中setjmp/longjmp函数并没有对进程信号屏蔽字的保护和恢复能力，
 *	而与之不同，sigsetjmp/siglongjmp函数对信号屏蔽字就就具有对信号屏蔽字
 *	保护和恢复的能力
 * */
#include "MyAPUE.h"
#include <signal.h>
#include <time.h>
#include <setjmp.h>

static void sig_usr1(int);
static void sig_alrm(int);
static sigjmp_buf jmpbuf;
static volatile sig_atomic_t canjmp;

int main(void)
{
	if (signal(SIGUSR1, sig_usr1) == SIG_ERR)
		err_sys("signal(SIGUSR1) error\n");
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error\n");

	pr_mask("starting main: ");	//无屏蔽
	if (sigsetjmp(jmpbuf, 1)) {	//此时由siglongjmp自动恢复无屏蔽的状态
		pr_mask("ending main: ");
		exit(0);
	}
	canjmp = 1;

	while (1)
		pause();
}

static void sig_usr1(int signo) {
	time_t starttime;
	if (canjmp == 0)
		return;

	pr_mask("starting sig_usr1: ");			//此时打印输出显示：SIGUSR1被自动的加入到了进程的信号屏蔽字
	alarm(3);
	starttime = time(NULL);
	while (1)
		if (time(NULL) > starttime + 5)
			break;							/*由于SIGARLM信号在5秒之内的循环内由内核传递给了进程，此时中断
											sig_usr1信号处理函数转而执行sig_alrm信号处理函数，此时打印输出
											显示：SIGUSR1 SIGALRM都被自动的加入到了进程的信号屏蔽字		*/
	pr_mask("finishing sig_usr1: ");		/*此时打印输出显示：只有SIGUSR1被自动的加入到了进程的信号屏蔽字
											SIGALRM被自动解除屏蔽										*/
	
	canjmp = 0;
	siglongjmp(jmpbuf, 1);
}

static void sig_alrm(int signo) {
	pr_mask("in sig_alrm: ");
}
