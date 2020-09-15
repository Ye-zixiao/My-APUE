#include "../include/MyAPUE.h"
#include <fcntl.h>

int main(int argc, char* argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pid_t pid;
	int fd;

	TELL_WAIT();
	if ((fd = open(argv[1], O_CREAT | O_WRONLY | O_APPEND, FILE_MODE)) < 0)
		err_sys("open erorr");
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		//子进程获取文件记录写锁
		write_lock(fd, 0, SEEK_SET, 0);
		TELL_PARENT(getppid());
		WAIT_PARENT();
		write(fd, "hello world from child\n", 24);
	}
	else {
		//父进程将文件描述符设置为非阻塞
		set_fl(fd, O_NONBLOCK);

		WAIT_CHILD();
		write(fd, "hello world from parent\n", 25);
		TELL_CHILD(pid);
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
	}

	exit(EXIT_SUCCESS);
}

/*
执行结果：
	$ ./main test

	# yexinhai @ localhost in ~/projects/APUE [22:11:47]
	$ cat test
	hello world from parent
	hello world from child

结果分析：
		在这个程序中，子进程遵守了建议性锁的规定，在执行写操作之前对文件进行了加锁
	，获得了记录写锁。但是父进程并没有遵守这个规定，而是直接调用了write函数将数据
	写入到不同文件描述符指向的同一个文件中。为了防止这个锁是强制性锁的可能，父进
	程还是实现调用了set_fl()函数将文件描述符标志设置为非阻塞以防止长时间在那里等
	待。
		可以发现，当一个进程即使遵守了建议性锁的规定，这个锁仍然不能防止其他具有
	写权力的进程通过调用write向其写入数据

*/
