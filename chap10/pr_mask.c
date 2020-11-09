#include "MyAPUE.h"
#include <signal.h>
#include <errno.h>

static void alrm_handler(int sig) {
	if (sig == SIGALRM)
		printf("received SIGALRM\n");
}

int main(void)
{
	sigset_t sigset;
	
	sigemptyset(&sigset);
	//if (sigaddset(&sigset, SIGALRM)==-1||sigaddset(&sigset,SIGUSR1))
	if (sigaddset(&sigset, SIGUSR1) == -1 || sigaddset(&sigset, SIGUSR2) == -1)
		err_sys("sigaddset error\n");
	if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1)
		err_sys("sigprocmask error\n");
	pr_mask("signal mask:");

	if (signal(SIGALRM, alrm_handler) == SIG_ERR)
		err_sys("signal error\n");
	alarm(3);
	pause();
	exit(0);
}
