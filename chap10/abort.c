#include"MyAPUE.h"

static void abort_sig(int sig) {
	if (sig == SIGABRT)
		printf("received SIGABRT\n");
}

int main(void)
{
	if (signal(SIGABRT, abort_sig) == SIG_ERR)
		err_sys("signal error\n");
	abort();
	printf("after abort\n");
	exit(EXIT_SUCCESS);
}