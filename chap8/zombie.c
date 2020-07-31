#include"MyAPUE.h"
#include<sys/wait.h>

int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		printf("hello world\n");
		_exit(0);
	}
	else {
		sleep(10);
		/*if (waitpid(pid, NULL, 0) != pid)*/
		if(waitid(P_PID,pid,NULL,WEXITED)!=0)
			err_sys("waitpid error\n");
		printf("child proccess(pid= %ld) terminated\n", (long)pid);
		sleep(10);
	}

	exit(0);
}

/*
在终端中输入：./main &;ps aux
	可以立即看到终止后的子进程马上就变成了僵尸进程Z，而此时父进程仍然在睡眠中无法通过waitpid取得
	其子进程的终止状态，这更不用提对该僵尸子进程的处理了。而当父进程从休眠期出来后它通过waitpid
	取得了相应的终止状态后就对子进程进行善后处理，最终再次运行ps aux也就看不到Z僵尸进程了

	waitpid处理前：
yexinhai   15993  0.0  0.0   2184   512 pts/1    S    20:42   0:00 ./main
yexinhai   15994  0.0  0.0  11916  3436 pts/1    R+   20:42   0:00 ps aux
yexinhai   15995  0.0  0.0      0     0 pts/1    Z    20:42   0:00 [main] <defunct>
yexinhai@localhost ~/p/APUE> ps auxchild proccess(pid= 15995) terminated

	waitpid处理后：
yexinhai   15993  0.0  0.0   2316   512 pts/1    S    20:42   0:00 ./main
yexinhai   16024  0.0  0.0  11916  3436 pts/1    R+   20:42   0:00 ps aux

*/