#include "apue.h"
#include <sys/wait.h>

static void sig_cld(int sig) {
	pid_t pid;

	printf("SIGCLD received\n");
	/*Linux之所以不会发生递归现象，是因为在使用signal函数设置SIGCLD信号处理程序时，
	 * 并且已经有子进程终止并向父进程发送了SIGCLD信号，它不会执行SIGCLD的信号处理
	 * 程序。即：Linux可以认为是不支持System V关于SIGCLD的第2）条语义*/
	if (signal(SIGCLD, sig_cld) == SIG_ERR)
		perror("signal error");
	if ((pid = wait(NULL)) < 0)
		perror("wait error");

	printf("pid = %d\n", pid);
}

int main(void)
{
	pid_t pid;

	if (signal(SIGCLD, sig_cld) == SIG_ERR)
		perror("signal errro");
	if ((pid = fork()) < 0)
		perror("fork error");
	else if (pid == 0) {
		sleep(2);
		_exit(0);
	}

	pause();
	exit(0);
}
