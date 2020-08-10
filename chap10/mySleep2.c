#include"MyAPUE.h"
#include<setjmp.h>


static jmp_buf env_alrm;

static void sig_alrm(int sig) {
	longjmp(env_alrm, 1);
}

static void sig_int(int sig) {
	volatile int k;

	printf("\nsig_int start...\n");
	for (int i = 0; i < 300000; i++)
		for (int j = 0; j < 4000; j++)
			k += i * j;
	printf("sig_int finished\n");
}

unsigned int
mySleep2(unsigned int seconds) {
	void (*po)(int);

	if ((po = signal(SIGALRM, sig_alrm)) == SIG_ERR)
		err_sys("signal error\n");
	if (setjmp(env_alrm) == 0) {
		alarm(seconds);
		pause();
	}

	if (signal(SIGALRM, po) == SIG_ERR)
		err_sys("signal error\n");
	return alarm(0);
}

int main(void)
{
	unsigned int unslept;

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error\n");
	unslept = mySleep2(2);						/*运行时立即键入Ctrl+C，使得马上程序进程收到中断信号SIGINT后，立即执行中断信号处理程序
												并长时间运行。此时由于闹钟信号发生使得SIGINT中断信号处理程序停止执行转而执行闹钟信号的
												处理程序，最终调用longjmp函数跨SIGINT的函数栈跳转回sleep函数，因此sig_int函数不会执行
												完毕*/
	printf("mySleep returned: %d\n", unslept);
	exit(0);
}
