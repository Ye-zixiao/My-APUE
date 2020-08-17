#include"../include/MyAPUE.h"

static void sig_chld(int signo) {
	if (signo == SIGCHLD) {
		printf("received signal SIGCHLD\n");
		printf("wait function returned value: %d\n",
			wait(NULL));
	}
}


int main(void)
{
	if (signal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("signal error\n");
	if (mysystem("date") < 0)
		err_sys("mysystem error\n");
	exit(EXIT_SUCCESS);
}
