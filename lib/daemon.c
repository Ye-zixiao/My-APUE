#include"../include/MyAPUE.h"
#include<fcntl.h>
#include<sys/resource.h>
#include<syslog.h>
#include<sys/stat.h>
#include<string.h>

#define LOCKMODE FILE_MODE

/**
 * 守护进程初始化函数.
 * @param str 当前守护进程名字
 */
void daemonize(const char* str) {
	struct sigaction sa;
	int fd0, fd1, fd2;
	struct rlimit rli;
	pid_t pid;

	umask(0);
	if (getrlimit(RLIMIT_NOFILE, &rli) != 0)
		err_quit("getrlimit error");
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", str);
	else if (pid != 0)
		exit(EXIT_SUCCESS);
	setsid();

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) != 0)
		err_quit("%s: can't ignore SIGHUP", str);
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", str);
	else if (pid != 0)
		exit(EXIT_SUCCESS);

	if (chdir("/") != 0)
		err_quit("%s: can't change directory to /", str);
	if (rli.rlim_max == RLIM_INFINITY)
		rli.rlim_max = 1024;
	for (int i = 0; i < rli.rlim_max; ++i)
		close(i);

	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(fd0);
	fd2 = dup(fd1);

	openlog(str, LOG_PERROR, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
		exit(EXIT_FAILURE);
	}
}


/**
 * 指定一个路径创建一个守护进程锁文件，确保只有单例守护进程在执行.
 * @param file 指定锁文件路径
 * @return 当前是否有守护进程副本在运行
 */
int singleDaemon(const char* file) {
	int fd;
	char buf[16];

	if ((fd = open(file, O_CREAT | O_RDWR, LOCKMODE)) < 0) {
		syslog(LOG_ERR, "can't open lockfile %s: %m", file);
		exit(EXIT_FAILURE);
	}

	if (lockfile(fd) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
#ifdef DEBUG
			fprintf("process %d can't lock this file\n", getpid());
#endif
			return 1;
		}
		syslog(LOG_ERR, "can't lock %s: %m", file);
		exit(EXIT_FAILURE);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%d", getpid());
	write(fd, buf, strlen(buf) + 1);
	return 0;
}
