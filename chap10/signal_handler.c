#include "apue.h"

static void sig_handler(int sig) {
	if (sig == SIGUSR1)
		printf("received SIGUSR1\n");
	else if (sig == SIGUSR2)
		printf("received SIGUSR2\n");
}

int main(void)
{
	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
		err_sys("signal error\n");
	if (signal(SIGUSR2, sig_handler) == SIG_ERR)
		err_sys("signal error\n");

	while (1)
		pause();
}
