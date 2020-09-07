#include"../include/MyAPUE.h"
#include<syslog.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/resource.h>

void daemonize(const char* cmd) {
	struct rlimit rl;
	struct sigaction sa;
	int fd0, fd1, fd2;
	pid_t pid;

	//1、设置文件模式创建屏蔽字
	umask(0);
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		err_quit("%s: can't get file limit", cmd);
	//2、调用fork，将父进程exit
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid > 0)
		exit(EXIT_SUCCESS);
	//3、调用setsid创建一个新的会话
	setsid();

	//4、新进程组（此时只有一个进程，即是会话首进程），先注册信号处理程序，
	//   防止进程组过会成为孤儿进程组时子子进程收到systemd的SIGHUP信号而终止
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) != 0)
		err_quit("%s: can't ignore SIGHUP", cmd);
	//5、再次创建一个新子进程，这个子进程绝对不会是会话首进程，因此不能获得
	//	 控制终端
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid > 0)
		exit(EXIT_SUCCESS);

	//6、改变工作目录为/
	if (chdir("/") != 0)
		err_quit("%s: can't chdir", cmd);
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	//7、关闭所有不需要的文件描述符
	for (int i = 0; i < rl.rlim_max; ++i)
		close(i);

	//8、使stdout、stdin、stderr指向/dev/null
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(fd0);
	fd2 = dup(fd0);

	//9、初始化log文件
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descritors %d %d %d",
			fd0, fd1, fd2);
		exit(EXIT_FAILURE);
	}
}


int main(void)
{
	daemonize("date");
	pause();
}
