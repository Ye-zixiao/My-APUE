#include"../include/MyAPUE.h"
#include<syslog.h>
#include<fcntl.h>
#include<string.h>

#define LOCKFILE "/tmp/mytestdaemon.pid"
#define CONFFILE "/tmp/mytestdaemon.conf"
#define LOGFILE "/tmp/mytestdaemon.outfile"
#define DEFAULTMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#ifdef BUFSIZE
#undef BUFSIZE
#define BUFSIZE 256
#endif

sigset_t mask;
char daemonbuf[BUFSIZE];


void reread(void) {
	int fd;
	int err;

	if ((fd = open(CONFFILE, O_RDONLY)) < 0) {
		syslog(LOG_INFO, "reread can't open CONFFILE");
		strcpy(daemonbuf, "NULL");
		return;
	}
	if ((err = read(fd, daemonbuf, BUFSIZE - 1)) < 0) {
		syslog(LOG_WARNING, "read of reread error");
		strcpy(daemonbuf, "NULL");
	}
	daemonbuf[err] = '\0';
	close(fd);
}


void* sigthread(void* args) {
	int err, signo;

	while (1) {
		if ((err = sigwait(&mask, &signo)) != 0) {
			syslog(LOG_ERR, "sigwait failed");
			exit(EXIT_FAILURE);
		}
		switch (signo) {
		case SIGHUP:
			syslog(LOG_INFO, "Re-reading configuration file");
			reread();
			break;
		case SIGQUIT:
			syslog(LOG_INFO, "got SIGTERM: exiting");
			exit(EXIT_SUCCESS);
		default:
			syslog(LOG_INFO, "unexpected singal %d\n", signo);
		}
	}
	return (void*)NULL;
}


int main(int argc, char* argv[])
{
	struct sigaction sa;
	pthread_t thd;
	char* cmd;
	int err;
	int fd;

	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	daemonize(cmd);
	if (singleDaemon(LOCKFILE) != 0) {
		syslog(LOG_ERR, "daemon already running");
		exit(EXIT_FAILURE);
	}

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigfillset(&mask);
	if (sigaction(SIGHUP, &sa, NULL) != 0)
		err_quit("%s: can't restore SIGHUP default", cmd);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
		err_exit(err, "SIG_BLOCK error");

	if ((err = pthread_create(&thd, NULL, sigthread, (void*)NULL)) != 0)
		err_exit(err, "can't create thread");


	if ((fd = open(LOGFILE, O_CREAT | O_WRONLY | O_TRUNC, DEFAULTMODE)) < 0)
		err_quit("%s: can't open LOFILE", cmd);
	reread();
	while (1) {
		write(fd, daemonbuf, strlen(daemonbuf));
		sleep(10);
	}

	exit(EXIT_SUCCESS);
}

/*
实验操作：
# yexinhai @ localhost in ~/projects/APUE [22:25:41]
$ ./main

# yexinhai @ localhost in ~/projects/APUE [22:27:42]
$ ps -efj|grep main
yexinhai   12727       1   12726   12726  2 22:27 ?        00:00:00 ./main
yexinhai   12742    9703   12741    9703  0 22:27 pts/0    00:00:00 grep --color=auto ... main

# yexinhai @ localhost in ~/projects/APUE [22:25:43]
$ cat /tmp/mytestdaemon.outfile
show me the code

# yexinhai @ localhost in ~/projects/APUE [22:25:47]
$ echo "I love Linux"> /tmp/mytestdaemon.conf

# yexinhai @ localhost in ~/projects/APUE [22:26:14]
$ cat /tmp/mytestdaemon.pid
12562%
# yexinhai @ localhost in ~/projects/APUE [22:26:28]
$ kill -1 12562

# yexinhai @ localhost in ~/projects/APUE [22:26:36]
$ cat /tmp/mytestdaemon.outfile
show me the code
show me the code
show me the code
show me the code
show me the code
show me the code

# yexinhai @ localhost in ~/projects/APUE [22:26:42]
$ cat /tmp/mytestdaemon.outfile
show me the code
show me the code
show me the code
show me the code
show me the code
show me the code
I love Linux
I love Linux

观察结果：
		从上面的可以看到：main的sid、pgid都是守护进程的父进程12726，而自己的pid为12727，
	也就说该进程自己不是进程组的组长进程，意味着它是不可能获得控制终端的。
		同时，在运行后马上它的父进程就变成了init进程（实际是systemd），显然它成为了一个
	用户态守护进程。且它的配置文件为了实验方便放在/tmp/mytestdaemon.conf文件中，为了能够
	展示实验结果，让该守护进程每隔一段时间向/tmp/mytesdaemon.outfile文件中输出获得的数据，
	且只允许任何时刻只能有该守护进程的单个实例在运行，故创建的锁文件放在/tmp/mytestdaemon.pid
		在实验中，如上所示，在一开始守护进程输出的数据是mytestdaemon.conf文件中的初始数据，
	在进行设置后并向该守护进程发送SIGHUP信号后，守护进程会重新读取其配置文件，可以看到输出数据
	的改变

*/
