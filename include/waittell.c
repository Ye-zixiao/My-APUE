#include"MyAPUE.h"

static sigset_t newmask, oldmask, zeromask;
static volatile sig_atomic_t sigflag;

static void sig_usr(int signo) {
	sigflag = 1;
}

void TELL_WAIT(void) {
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("signal error\n");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("signal error\n");
	sigemptyset(&newmask);
	sigemptyset(&zeromask);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	//是当前进程对SIGUSRx信号进行阻塞
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) == -1)
		err_sys("sigprocmask error\n");
}

void TELL_PARENT(pid_t pid) {
	kill(pid, SIGUSR2);
}

void TELL_CHILD(pid_t pid) {
	kill(pid, SIGUSR1);
}

void WAIT_CHILD(void) {
	while (sigflag == 0)
		sigsuspend(&zeromask);//暂时解除阻塞并休眠，直到子进程发送SIGUSR2信号过来
	sigflag = 0;

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) == -1)
		err_sys("sigprocmask error\n");
}

void WAIT_PARENT(void) {
	while (sigflag == 0)
		sigsuspend(&zeromask);
	sigflag = 0;

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) == -1)
		err_sys("sigprocmask error\n");
}