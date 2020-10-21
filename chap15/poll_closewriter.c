/**
 * 父子进程原通过管道进行连接，实验的内容是：让父进程关闭其在管道中的
 * 写端文件描述符，测试下poll在此时的表现
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/poll.h>

int main(void)
{
	int fds[2];
	pid_t pid;

	if (pipe(fds) == -1)
		err_sys("pipe error");

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		struct pollfd pfds[1];
		char buf[BUFSIZ];
		size_t nread;

		close(fds[1]);
		pfds[0].fd = fds[0];
		pfds[0].events = POLLIN | POLLHUP;
		pfds[0].revents = 0;
		for (;;) {
			if (poll(pfds, 1, -1) == -1)
				err_sys("poll error");
			for (int i = 0; i < 1; ++i) {
				if (pfds[i].revents & (POLLIN | POLLHUP)) {
					pfds[i].revents = 0;
					if ((nread = read(pfds[i].fd, buf, BUFSIZ)) < 0)
						err_sys("read error");
					else if (nread == 0) {
						/* 若子进程从与父进程连接的管道中读取到文件结束标志，则表示父进程
							关闭了与子进程通过管道的连接。在Linux中若管道的写端描述符被关
							闭，则处在另一读端的poll函数会返回POLLHUP事件(而不是像select一
							样被当成可读事件一样返回)，所以events中仅仅设置POLLIN是会导致
							poll这端的进程一直阻塞的！😂*/
						fprintf(stderr, "child: parent close his pipe writer fd, so I exit!\n");
						exit(EXIT_FAILURE);
					}
					if (write(STDOUT_FILENO, buf, nread) != nread)
						err_sys("write error");
				}
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
