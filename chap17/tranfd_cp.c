/**
 * 该程序用来验证子进程打开文件后向父进程传送文件
 * 描述符的结果就是父子进程同时共享一个文件打开表项
 */
#include "../include/MyAPUE.h"
#include <fcntl.h>
#include <sys/socket.h>

int main(int argc, char* argv[])
{
	int pid, fd;
	int fdpair[2];

	if (socketpair(AF_UNIX,SOCK_STREAM,0,fdpair) == -1)
		err_sys("fd_pipe error");

	TELL_WAIT();
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		close(fdpair[1]);

		if ((fd = open("/etc/passwd", O_RDONLY)) == -1)
			err_sys("open error");
		printf("get here?\n");
		if (send_fd(fdpair[0], fd) < 0)
			err_sys("send_fd error");
		printf("get here2?\n");

		WAIT_PARENT();
		lseek(fd, 10, SEEK_SET);
		TELL_PARENT(getppid());

		WAIT_PARENT();
		lseek(fd, 20, SEEK_SET);
		TELL_PARENT(getppid());

		exit(EXIT_SUCCESS);
	}
	else {
		close(fdpair[0]);
		if ((fd = recv_fd(fdpair[1], write)) < 0)
			err_sys("recv_fd error");
		printf("Parent: got fd: %d, seek: %lld\n", fd, (long long)lseek(fd, 0, SEEK_CUR));
		TELL_CHILD(pid);

		WAIT_CHILD();
		printf("Parent: got fd: %d, seek: %lld(should be 10)\n", fd, (long long)lseek(fd, 0, SEEK_CUR));
		TELL_CHILD(pid);

		WAIT_CHILD();
		printf("Parent: seek after child changed it second time: %lld(should be 20)\n", (long long)lseek(fd, 0, SEEK_CUR));

		exit(EXIT_SUCCESS);
	}
}
