#include "../include/MyAPUE.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
	struct stat statbuf;
	char buf[5];
	pid_t pid;
	int fd;

	if (argc != 2) {
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//创建一个文件并设置其文件记录锁类型为强制性锁
	if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0)
		err_sys("open error");
	if (write(fd, "abcdef", 6) != 6)
		err_sys("write error");

	if (fstat(fd, &statbuf) < 0)
		err_sys("fstat error");
	if (fchmod(fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
		err_sys("fchmod error");

	//父进程持有记录写锁，子进程则试图去获取记录读锁和执行read函数
	TELL_WAIT();
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid != 0) {
		if (write_lock(fd, 0, SEEK_SET, 0) < 0)
			err_sys("write_lock error");
		TELL_CHILD(pid);
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
	}
	else {
		WAIT_PARENT();
		set_fl(fd, O_NONBLOCK);

		if (read_lock(fd, 0, SEEK_SET, 0) != -1)
			err_sys("child: read_lock succeeded");
		printf("read_lock of already-locked region returns %d\n", errno);

		if (lseek(fd, 0, SEEK_SET) == -1)
			err_sys("lseek error");
		//若支持强制性锁，则子进程的非阻塞read会返回-1，并设置errno为EAGAIN
		if (read(fd, buf, 2) < 0)
			err_ret("read failed (mandatory locking works)");
		else
			printf("read OK(no mandatory locking), buf = %2.2s\n",
				buf);
	}

	exit(EXIT_SUCCESS);
}
