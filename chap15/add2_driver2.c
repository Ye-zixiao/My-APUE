/**
 * 改写P443中的add2_driver.c程序，使得父进程能够以标准I/O
 * 的方式管理对管道的读写
 */
#include "../include/MyAPUE.h"

#define MAXLINE 64

static void sig_pipe(int signo) {
	fprintf(stderr, "caught SIGPIPE\n");
	exit(EXIT_FAILURE);
}


int main(void)
{
	FILE* fp_PipeIn, * fp_PipeOut;
	int fds1[2], fds2[2];
	char buf[MAXLINE];
	pid_t pid;

	if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
		err_sys("signal error");
	if (pipe(fds1) == -1 || pipe(fds2) == -1)
		err_sys("pipe error");

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid > 0) {
		close(fds1[0]);
		close(fds2[1]);
		/* 以标准I/O的方式打开指向管道读写端的文件描述符 */
		if ((fp_PipeOut = fdopen(fds1[1], "w")) == NULL)
			err_sys("fdopen error");
		if ((fp_PipeIn = fdopen(fds2[0], "r")) == NULL)
			err_sys("fdopen error");

		while (fgets(buf, MAXLINE, stdin) != NULL) {
			if (fputs(buf, fp_PipeOut) == EOF)
				err_sys("fputs error");
			//当然最好的方式是在前面调用setvbuf()改变流缓冲区类型
			fflush(fp_PipeOut);
			if (fgets(buf, MAXLINE, fp_PipeIn) == NULL)
				err_sys("fgets error");
			if (fputs(buf, stdout) == EOF)
				err_sys("fputs error");
		}
		if (ferror(stdin))
			err_sys("fgets error");
	}
	else {
		close(fds1[1]);
		close(fds2[0]);
		if (fds1[0] != STDIN_FILENO) {
			if (dup2(fds1[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error");
			close(fds1[0]);
		}
		if (fds2[1] != STDOUT_FILENO) {
			if (dup2(fds2[1], STDOUT_FILENO) != STDOUT_FILENO)
				err_sys("dup2 error");
			close(fds2[1]);
		}

		if (execl("./add2_", "add2", (char*)NULL) == -1)
			err_sys("execl error");
	}

	exit(EXIT_SUCCESS);
}
