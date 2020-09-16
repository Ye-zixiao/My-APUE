#include "../include/MyAPUE.h"
#include <fcntl.h>
#include <syslog.h>
#include <string.h>

#define DEBUG
#define LOCKFILE "/tmp/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int already_running(void) {
	int fd;
	char buf[16];

	openlog("already_running", LOG_PERROR, LOG_DAEMON);
	fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
	if (fd < 0) {
		syslog(LOG_ERR, "open error");
		exit(EXIT_FAILURE);
	}

	//使用文件锁，使得只有单个守护进程副本能够访问同一个文件
	if (lockfile(fd) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
#ifdef DEBUG
			fprintf(stderr, "lockfile error: %s\n", strerror(errno));
#endif
			return 1;
		}
		syslog(LOG_ERR, "can't lock %s: %m", LOCKFILE);
		exit(EXIT_FAILURE);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%d", getpid());
	write(fd, buf, strlen(buf) + 1);
	return 0;
}


int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		sleep(1);
		printf("child: %d\n", getpid());
		already_running();
	}
	else {
		printf("parent: %d\n", getpid());
		already_running();
		if (waitpid(pid, NULL, 0) != pid)
			err_quit("waitpid error");
	}

	exit(EXIT_SUCCESS);
}

/*
实验结果：
	$ ./main
	child: 6318
	parent: 6317
	lockfile error: Resource temporarily unavailable//若开启DEBUG

	# yexinhai @ localhost in ~/projects/APUE [17:36:52]
	$ cat ~/daemon.pid
	6317%
由上面的结果可以看出，由于父进程先运行并创建/打开了指定文件，它当然可以获得文件锁并
该文件中写入数据；但是当子进程运行时，由于父进程在等它死，所以该文件的文件锁一直持有在
父进程手上，故子进程获取不到，只能失败返回。这样通过文件记录锁的方式我们就保证了只有
单个守护进程副本能够真正意义上在运行
*/
