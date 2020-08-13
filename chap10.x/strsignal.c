#include"../include/MyAPUE.h"
#include<string.h>

static void sig_int(int sig) {
	if (sig == SIGINT)
		psignal(sig, "\nReceived signal");
}

static void sig_quit(int sig) {
	if (sig == SIGQUIT)
		fprintf(stderr, "\nReceived signal: %s\n",
			strsignal(sig));
}

int main(void)
{
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error\n");
	if (signal(SIGQUIT, sig_quit) == SIG_ERR)
		err_sys("signal error\n");

	while (1)
		pause();
	exit(EXIT_SUCCESS);
}
