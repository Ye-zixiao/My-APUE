#include"apue.h"
#include<errno.h>

static void sig_hup(int signo) {
	printf("SIGHUP received, pid = %ld\n", (long)getpid());
}

static void pr_ids(char* name) {
	//		进程名  进程id  父进程id  进程组id  所属会话id  前台进程组id
	printf("%-6s: pid= %4d, ppid= %4d, pgid= %4d, sid= %d, tpgid= %d\n",
		name, getpid(), getppid(), getpgrp(), getsid(getpid()), tcgetpgrp(STDOUT_FILENO));
	fflush(stdout);
}

int main(void)
{
	char ch;
	pid_t pid;

	pr_ids("parent");
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid > 0) {
		//sleep(5);
	}
	else {
		/*
			因为posix要求向每一个新孤儿进程组中处于停止状态的每一个进程先发送SIGHUP挂断信号，
			然后接着向其发送SIGCONT继续信号，因为进程默认的情况下受到挂断信号的反应是终止当前
			进程，所以我们在实验中需要自己定义一个信号处理函数以使得进程不会终止
		*/
		//虽然fork没有像vfork那样保证一定是子进程先执行，但是一般来说确实是子进程先执行
		pr_ids("child");
		sleep(3);			//尽量保证父进程先终止	
		signal(SIGHUP, sig_hup);
		kill(getpid(), SIGTSTP);
		pr_ids("child");
		if (read(STDIN_FILENO, &ch, 1) != 1)
			err_sys("read error %d on controlling TTY", errno);
	}

	exit(0);
}

/*
尽量在bash上执行，fish的工作控制有问题，尤其是stty

[22:23 yexinhai@localhost APUE]$ ./main
parent: pid= 9488, ppid= 8451, pgid= 9488, sid= 943, tpgid= 9488
child : pid= 9489, ppid= 9488, pgid= 9488, sid= 943, tpgid= 9488
[22:23 yexinhai@localhost APUE]$ child : pid= 9489, ppid=    1, pgid= 9488, sid= 943, tpgid= 8451
read error 5 on controlling TTY: Input/output error

*/