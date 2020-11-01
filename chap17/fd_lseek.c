/**
 * 子进程打开指定文件并将其文件描述符传送给父进程，之后子进程
 * 对打开文件表中的文件偏移量进行修改，而父进程检查自己的文件
 * 偏移量是否被修改（即验证两进程中的文件描述符是否同指向同一
 * 个文件打开表项）
 */
#include "../include/MyAPUE.h"

int main(int argc, char* argv[])
{
	struct stat statbuf;
	int fds[2], fd;
	pid_t pid;

	if (argc != 2)
		err_quit("usage: %s <filename>", argv[0]);
	if (fd_pipe(fds) == -1)
		err_sys("fd_pipe error");

	TELL_WAIT();
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		close(fds[0]);

		if (setvbuf(stdout, NULL, _IOLBF, 0) == -1)
			err_sys("setvbuf error");
		//子进程打开文件并发送文件描述符
		if ((fd = open(argv[1], O_RDONLY)) == -1)
			err_sys("open error");
		if (fstat(fd, &statbuf) == -1)
			err_sys("fstat error");
		if (send_fd(fds[1], fd) < 0)
			err_sys("send_fd error");
		printf("child : send fd %d to parent, file offset %ld\n", fd, lseek(fd, 0, SEEK_CUR));
		TELL_PARENT(getppid());

		/* 子进程试图进行两次文件打开表偏移量修改 */
		WAIT_PARENT();
		printf("child : set file offset to %ld\n", lseek(fd, statbuf.st_size / 2, SEEK_SET));
		TELL_PARENT(getppid());

		WAIT_PARENT();
		printf("child : set file offset to %ld\n", lseek(fd, statbuf.st_size / 3, SEEK_SET));
		TELL_PARENT(getppid());
	}
	else {
		close(fds[1]);
		if ((fd = recv_fd(fds[0], write)) < 0)
			err_sys("recv_fd error");

		/* 父进程在每一次子进程修改完打开文件表文件偏移量后进行检查 */
		WAIT_CHILD();
		printf("parent: received fd %d, offset %ld\n", fd, lseek(fd, 0, SEEK_CUR));
		TELL_CHILD(pid);

		WAIT_CHILD();
		printf("parent: current offset %ld\n", lseek(fd, 0, SEEK_CUR));
		TELL_CHILD(pid);

		WAIT_CHILD();
		printf("parent: current offset %ld\n", lseek(fd, 0, SEEK_CUR));
	}

	exit(EXIT_SUCCESS);
}
