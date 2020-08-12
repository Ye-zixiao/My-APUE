/*
 * 该程序演示了为什么system函数需要进行信号处理
 *
 * */
#include"../include/MyAPUE.h"

static void sig_int(int signo) {
	printf("caught SIGINT\n");
}

static void sig_chld(int signo) {
	printf("caught SIGCHLD\n");
}

int main(void)
{
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error\n");
	if (signal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("signal error\n");
	if (System("/bin/ed") < 0)
		err_sys("System() error\n");
	exit(0);
}
