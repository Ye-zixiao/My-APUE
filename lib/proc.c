#include"../include/MyAPUE.h"


int System(const char* cmdstring) {
	if (cmdstring == NULL)
		return 1;

	pid_t pid;
	int status;
	if ((pid = fork()) < 0)
		status = -1;
	else if (pid == 0) {
		/*
			System--->fork-exec出shell------>shell fork-exec出cmdstring（读起来怪怪的😂）
		*/
		//execl("/usr/bin/sh", "sh", "-c", cmdstring, (char*)NULL);
		execl("/bin/sh", "sh", "-c", cmdstring, (char*)NULL);
		_exit(127);
	}
	else {
		while (waitpid(pid, &status, 0) < 0) {
			if (errno == EINTR) {
				status = -1;
				break;
			}
		}
	}

	return status;
}
/*
1、fork、waitpid失败，system返回-1
2、execl失败返回127
3、正常返回那么就是shell的退出状态
*/


/*
* ------------------------------------------------
*	一个能够正确处理信号的system函数
* ------------------------------------------------
*/
int mysystem(const char* cmdstring) {
	struct sigaction ignore_act, oldint_act, oldquit_act;
	sigset_t newmask, oldmask;
	pid_t pid;
	int status;

	if (cmdstring == NULL)
		return -1;

	/*
		保存进程原来的信号处理方式，然后更新信号处理方式:
		1.更新保存对SIGINT/SIGQUTI的信号处理
		2.更新保存对SIGCHLD的信号屏蔽字
	*/
	ignore_act.sa_handler = SIG_IGN;
	ignore_act.sa_flags = 0;
	sigemptyset(&ignore_act.sa_mask);
	if (sigaction(SIGINT, &ignore_act, &oldint_act) == -1)
		err_sys("sigaction error\n");
	if (sigaction(SIGQUIT, &ignore_act, &oldquit_act) == -1)
		err_sys("sigaction error\n");
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) == -1)
		err_sys("sigprocmask error\n");

	/*
		fork-exec生成子进程
	*/
	if ((pid = fork()) < 0)
		status = -1;
	else if (pid == 0) {
		sigaction(SIGINT, &oldint_act, NULL);
		sigaction(SIGQUIT, &oldquit_act, NULL);
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
		execl("/bin/sh", "sh", "-c", cmdstring, (char*)NULL);
		_exit(127);
	}
	else {
		while (waitpid(pid, &status, 0) != pid) {
			if (errno == EINTR) {
				status = -1;
				break;
			}
		}
	}

	/*
		恢复system调用进程原来的信号处理方式和信号屏蔽字
	*/
	if (sigaction(SIGINT, &oldint_act, NULL) == -1)
		err_sys("sigaction error\n");
	if (sigaction(SIGQUIT, &oldquit_act, NULL) == -1)
		err_sys("sigaction error\n");
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) == -1)
		err_sys("sigprocmask error\n");
	/*
		POSIX.1规定wait若取得了子进程的终止状态，则之前阻塞的SIGCHLD信号
		不再像当前进程递交。但是这个规定在Linux上并没有实现
	*/
	return status;
}


void Abort(void) {
	sigset_t mask;
	struct sigaction action;

	sigaction(SIGABRT, NULL, &action);
	if (action.sa_handler == SIG_IGN) {		/*若进程对SIGABRT信号进行忽略，则设置进程对该信号采取默认处理方式*/
		action.sa_handler = SIG_DFL;
		sigaction(SIGABRT, &action, NULL);
		//printf("first get here??\n");
	}
	if (action.sa_handler == SIG_DFL)
		fflush(NULL);

	sigfillset(&mask);
	sigdelset(&mask, SIGABRT);
	sigprocmask(SIG_SETMASK, &mask, NULL);	//只接收SIGABRT信号
	raise(SIGABRT);

	/*
	*	若原进程对SIGABRT信号采取忽略或者捕获该信号处理方式，那么调用raise函数
	*	向该进程发送SIGABRT信号会使得其忽略或先调用信号处理程序后返回而进入下面
	*	的代码片段
	*/
	//printf("second get here???\n");
	fflush(NULL);
	action.sa_handler = SIG_DFL;
	sigaction(SIGABRT, &action, NULL);		//对SIGABRT信号采取默认处理方式(终止)
	sigprocmask(SIG_SETMASK, &mask, NULL);
	kill(getpid(), SIGABRT);
	exit(EXIT_FAILURE);
}


static void sig_alrm(int signo) {
	//...nothing to do
}

unsigned int Sleep(unsigned int seconds) {
	struct sigaction newact, oldact;
	sigset_t newmask, oldmask, suspend_mask;
	unsigned int unslept;

	/*
		保存并更新信号处理方式和信号屏蔽字：
			之所以要阻塞SIGALRM是为了防止在休眠前收到SIGALRM进入信号处理程序，
		然后返回后在进入休眠函数（pause或sigsuspend），导致竞态条件，永久的进
		入休眠状态
	*/
	newact.sa_flags = 0;
	sigemptyset(&newact.sa_mask);
	newact.sa_handler = sig_alrm;
	sigaction(SIGALRM, &newact, &oldact);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGALRM);
	sigprocmask(SIG_BLOCK, &newmask, &oldmask);

	/*
		定时、等待信号并休眠
	*/
	alarm(seconds);
	suspend_mask = oldmask;
	sigdelset(&suspend_mask, SIGALRM);
	sigsuspend(&suspend_mask);//原子的修改信号屏蔽字并休眠等待信号
	unslept = alarm(0);

	/*
		恢复信号处理方式以及信号屏蔽字
	*/
	sigaction(SIGALRM, &oldact, NULL);
	sigprocmask(SIG_SETMASK, &oldmask, NULL);
	return unslept;
}
