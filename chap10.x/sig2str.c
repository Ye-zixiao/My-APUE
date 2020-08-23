#include"../include/MyAPUE.h"
#include<string.h>

extern const char* const sys_siglist[];

int Sig2Str(int signo, char* str) {
	if (signo < 0 || signo >= _NSIG)
		return -1;
	strcpy(str, sys_siglist[signo]);
	return 0;
}

int main(void)
{
	char buf[SBUFSIZE];

	if (Sig2Str(SIGALRM, buf) != 0)
		err_sys("Sig2Str error\n");
	printf("SIGALRM ----> %s\n", buf);
	exit(EXIT_SUCCESS);
}
