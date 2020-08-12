#include"../include/MyAPUE.h"

static void abrt_sig(int sig) {
	if (sig == SIGABRT)
		printf("received signal SIGABRT\n");
}

int main(void)
{
	if (signal(SIGABRT, abrt_sig) == SIG_ERR)
		err_sys("signal error\n");
	Abort();
	exit(0);
}
