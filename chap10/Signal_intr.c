#include "MyAPUE.h"
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include <sys/time.h>

typedef void Sig_handler(int);
static jmp_buf env_buf;

Sig_handler* Signal_intr(int signo, Sig_handler* func);
static void alrm_handler(int sig);
void print_now(void);


int main(void)
{
	if (Signal_intr(SIGALRM, alrm_handler) == SIG_ERR)
		err_sys("Signal error\n");

	print_now();
	if (setjmp(env_buf) == 0) {
		alarm(5);
		pause();
	}
	print_now();
	
	exit(0);
}


/*===================================================*
*			 function defination
* ===================================================*
*/
Sig_handler*
Signal_intr(int signo, Sig_handler* sig_handler) {
	struct sigaction act, old_act;

	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
#if defined(SA_INTERRUPT)
	/*
		禁止由这些信号导致中断的系统调用自动重启动
	*/
	act.sa_flags |= SA_INTERRUPT;
#endif
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
