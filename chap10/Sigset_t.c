#include "MyAPUE.h"
#include <signal.h>
#include <errno.h>

typedef unsigned long int Sigset_t;

#define SIGBAD(signo) ((signo)<=0||(signo)>=NSIG)
#define Sigemptyset(ptr) (*(ptr)=0)
#define Sigfullset(ptr) (*(ptr)=~(sigset_t)0,0)

int Sigaddset(Sigset_t* set, int signo) {
	if (SIGBAD(signo)) {
		errno = EINVAL;
		return -1;
	}
	*set |= (1 << (signo - 1));
	return 0;
}

int Sigdelset(Sigset_t* set, int signo) {
	if (SIGBAD(signo)) {
		errno = EINVAL;
		return -1;
	}
	*set &= ~(1 << (signo - 1));
	return 0;
}

int Sigismember(Sigset_t* set, int signo) {
	if (SIGBAD(signo)) {
		errno = EINVAL;
		return -1;
	}
	return (*set & 1 << (signo - 1)) != 0;
}

int main(void)
{
	Sigset_t mysigset;
	Sigemptyset(&mysigset);
	if (Sigaddset(&mysigset, SIGALRM) == -1)
		err_sys("Sigaddset error\n");
	if (Sigismember(&mysigset, SIGALRM))
		printf("SIGALRM is the memeber of mysigset\n");
	if (Sigismember(&mysigset, SIGINT)==0)
		printf("SIGINT is not the member of mysigset\n");

	return 0;
}
