#include "../include/MyAPUE.h"

#define MAXLINE 1024

int main(void)
{
	int ret;
	int fds[2];
	pid_t pid;
	char buf[MAXLINE];

	if (pipe(fds) < 0)
		err_sys("pipe error");
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		close(fds[1]);
		ret = read(fds[0], buf, MAXLINE);
		write(STDOUT_FILENO, buf, ret);
	}
	else {
		close(fds[0]);
		write(fds[1], "hello world\n", 13);
	}

	exit(EXIT_SUCCESS);
}
