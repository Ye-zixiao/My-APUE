/**
 * 父子进程原通过管道进行连接，实验的内容是：让父进程关闭其在管道中的
 * 写端文件描述符，测试下select在此时的表现
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/select.h>

int main(void)
{
	int fds[2];
	pid_t pid;

	if (pipe(fds) == -1)
		err_sys("pipe error");

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		char buf[BUFSIZ];
		fd_set rset;
		size_t nread;

		close(fds[1]);
		for (;;) {
			FD_ZERO(&rset);
			FD_SET(fds[0], &rset);

			if (select(fds[0] + 1, &rset, NULL, NULL, NULL) == -1)
				err_sys("select error");
			if (FD_ISSET(fds[0], &rset)) {
				memset(buf, 0, sizeof(buf));
				if ((nread = read(fds[0], buf, BUFSIZ)) < 0)
					err_sys("read error");
				else if (nread == 0) {
					/* 若子进程从管道读端读取到文件结束标志，则表示
						父进程关闭了与子进程通过管道的连接 */
					fprintf(stderr, "child: parent close his pipe writer fd, so I exit!\n");
					exit(EXIT_FAILURE);
				}
				if (write(STDOUT_FILENO, buf, nread) != nread)
					err_sys("write error");
			}
		}
	}
	else {
		close(fds[0]);
		if (write(fds[1], "before parent close pipe writer\n", 32) != 32)
			err_sys("write error");
		sleep(2);
		close(fds[1]);
		fprintf(stderr, "parent: I close pipe writer\n");
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
	}

	exit(EXIT_SUCCESS);
}
