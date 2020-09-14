/**
 * 使用信号处理程序来解决守护进程配置文件重读的问题 
 */
#include "../include/MyAPUE.h"
#include <fcntl.h>
#include <syslog.h>
#include <string.h>

extern int lockfile(int);
extern int singleDaemon(const char*);

#define LOCKFILE "/tmp/mytestdaemon.pid"
#define CONFFILE "/tmp/mytestdaemon.conf"
#define LOGFILE "/tmp/mytestdaemon.outfile"
#define DEFAULTMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#ifdef BUFSIZE
#undef BUFSIZE
#define BUFSIZE 256
#else
#define BUFSIZE 256
#endif

char daemonbuf[BUFSIZE];

/**
 * 重读配置文件.
 */
void reread(void) {
	int fd, err;

	if ((fd = open(CONFFILE, O_RDONLY)) < 0) {
		syslog(LOG_INFO, "reread can't open CONFFILE");
		strcpy(daemonbuf, "NULL");
		daemonbuf[4] = '\0';
		close(fd);
		return;
	}
	if ((err = read(fd, daemonbuf, BUFSIZE)) < 0) {
		syslog(LOG_WARNING, "read of reread error");
		strcpy(daemonbuf, "NULL");
		daemonbuf[4] = '\0';
		close(fd);
		return;
	}
	daemonbuf[err] = '\0';
	close(fd);
}


/**
 * 处理SIGQUIT的信号处理程序(这里我用SIGQUIT代替了书中
 * 的SIGTERM，主要是为了实验方便).
 * @param signo 递交信号编码
 */
void sigquit(int signo) {
	syslog(LOG_INFO, "got SIGTERM;exiting...");
	exit(EXIT_SUCCESS);
}


/**
 * 处理SIGHUP的信号处理程序.
 * @param signo 递交信号编码
 */
void sighup(int signo) {
	syslog(LOG_INFO, "Re-reading configuration file");
	reread();
}


int main(int argc, char* argv[])
{
	struct sigaction sa;
	char* cmd;
	int fd;

	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	//守护进程单实例初始化
	daemonize(cmd);
	if (singleDaemon(LOCKFILE) != 0) {
		syslog(LOG_ERR, "daemon already running");
		exit(EXIT_FAILURE);
	}

	//设置SIGQUIT和SIGHUP的信号处理程序
	sa.sa_handler = sigquit;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGQUIT);
	sa.sa_flags = 0;
	if (sigaction(SIGQUIT, &sa, NULL) != 0) {
		syslog(LOG_ERR, "can't catch SIGQUIT: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = sighup;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) != 0) {
		syslog(LOG_ERR, "can't catch SIGHUP: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	//打开log文件以使实验结果比较明显
	if ((fd = open(LOGFILE, O_CREAT | O_WRONLY | O_TRUNC, DEFAULTMODE)) < 0) {
		syslog(LOG_ERR, "can't open LOGFILE");
		exit(EXIT_FAILURE);
	}
	reread();
	while (1) {
		write(fd, daemonbuf, strlen(daemonbuf));
		sleep(10);
	}
	
	exit(EXIT_SUCCESS);
}
