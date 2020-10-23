#include "open.h"
#include <string.h>
#include <sys/uio.h>


/**
 * 向服务进程发送打开指定文件请求，服务子进程则对应地打开相应
 * 地文件，并传送回对应的文件描述符
 */
int csopen(char* pathname, int oflag) {
	static int fds[2] = { -1,-1 };
	struct iovec iov[3];
	char buf[10];
	pid_t pid;
	int len;

	if (fds[0] < 0) {
		if (fd_pipe(fds) < 0) {
			err_ret("fd_pipe error");
			return -1;
		}
		if ((pid = fork()) < 0) {
			err_ret("fork error");
			return -1;
		}
		else if (pid == 0) {
			close(fds[0]);
			if (fds[1] != STDIN_FILENO &&
					dup2(fds[1], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error");
			if (fds[1] != STDOUT_FILENO &&
					dup2(fds[1], STDOUT_FILENO) != STDOUT_FILENO)
				err_sys("dup2 error");

			if (execl("./server", "server", (void*)NULL) == -1)
				err_sys("execl error");
		}
		close(fds[1]);
	}
	/* 父进程发送"open <pathname> <oflag>"消息给服务子进程 */
	sprintf(buf, " %d", oflag);
	iov[0].iov_base = CL_OPEN" ";//其中的'\0'会被忽略
	iov[0].iov_len = strlen(CL_OPEN) + 1;
	iov[1].iov_base = pathname;
	iov[1].iov_len = strlen(pathname);
	iov[2].iov_base = buf;
	iov[2].iov_len = strlen(buf) + 1;
	len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len;
	if (writev(fds[0], iov, 3) != len) {
		err_ret("writev error");
		return -1;
	}
	
	return recv_fd(fds[0], write);
}