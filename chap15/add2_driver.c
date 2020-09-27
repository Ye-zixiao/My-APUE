#include "../include/MyAPUE.h"
#include <string.h>

#define MAXLINE 1024

static void sig_pipe(int signo) {
	printf("SIGPIPE caught\n");
	exit(EXIT_FAILURE);
}

int main(void)
{
	int val, fd1[2], fd2[2];
	char line[MAXLINE];
	pid_t pid;

	//设置信号处理程序并创建与子进程之间的两条管道
	if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
		err_sys("signal error");
	if (pipe(fd1) < 0 || pipe(fd2) < 0)
		err_sys("pipe error");
	
	//创建子进程
	if ((pid = fork()) < 0) {
		for (int i = 0; i < 2; i++) {
			close(fd1[i]); close(fd2[i]);
		}
		err_sys("fork error");
	}
	else if (pid == 0) {
		close(fd1[1]);
		close(fd2[0]);

		if (fd1[0] != STDIN_FILENO) {
			if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error");
			close(fd1[0]);
		}
		if (fd2[1] != STDOUT_FILENO) {
			if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
				err_sys("dup2 error");
			close(fd2[1]);
		}

		if (execl("./add2_", "add2_", (char*)NULL) < 0)
		//if (execl("./add2", "add2", (char*)NULL) < 0)
			err_sys("execl error");
	}
	else {
		close(fd1[0]);
		close(fd2[1]);

		/**
		 * 父进程读取stdin并将其通过一条管道发送给协同进程，然后从另一条
		 * 管道中读取协同进程处理好后的数据，并将其输出到自己的stdout中
		 */
		while (fgets(line, MAXLINE, stdin) != NULL) {
			val = strlen(line);
			if (write(fd1[1], line, val) != val)
				err_sys("write error");
			if ((val = read(fd2[0], line, MAXLINE)) <= 0) {
				if (val == 0) {
					err_msg("child coprocess closed pipe/terminated");
					break;
				}
				err_sys("read error");
			}
			line[val] = '\0';
			if (fputs(line, stdout) == EOF)
				err_sys("fputs error");
		}

		if (ferror(stdin))
			err_sys("ferror error");
	}

	exit(EXIT_SUCCESS);
}
