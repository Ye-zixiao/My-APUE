#include"../include/MyAPUE.h"

static void sig_tstp(int signo) {
	sigset_t mask;

	/*
		停止进程前的处理工作
	*/
	fputs("\nEntering into stopped state...\n", stdout);
	fflush(stdout);

	/*
		因为进入信号处理程序会阻塞再次接收到的同样信号，
		所以需要接触对SIGTSTP信号的阻塞，然后向自己发送
		SIGTSTP以使当前进程停止
	*/
	sigemptyset(&mask);
	sigaddset(&mask, SIGTSTP);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	signal(SIGTSTP, SIG_DFL);
	raise(SIGTSTP);

	/*
		当接收到SIGCONT准备继续工作的时候，恢复对SIGTSTP原来
		的信号处理方式
	*/
	signal(SIGTSTP, sig_tstp);
}

static void sig_cont(int signo) {
	fputs("Continue entering...\n", stdout);
	fflush(stdout);
}


int main(void)
{
	int val;
	char buf[BUFSIZE];

	if (signal(SIGTSTP, SIG_IGN) == SIG_DFL)
		signal(SIGTSTP, sig_tstp);
	if (signal(SIGCONT, sig_cont) == SIG_ERR)
		err_sys("signal error\n");

	while ((val = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, val) != val)
			err_sys("write error\n");
	if (val < 0)
		err_sys("read error\n");
	exit(EXIT_SUCCESS);
}
