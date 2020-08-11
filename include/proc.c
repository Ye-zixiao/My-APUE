#include"MyAPUE.h"

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