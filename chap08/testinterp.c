#include"MyAPUE.h"
#include<sys/wait.h>

int main(void)
{
	pid_t pid;
	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		/*if (execl("/home/yexinhai/my-apue/chap8/tell",		//1、若pathname为二进制可执行文件
				"arg1", "arg2", "arg3", (char*)0) == -1)*/
		//if (execl("/home/yexinhai/my-apue/chap8/testinterp.sh",	//2、若pathname为解释器文件
		//	"arg1", "arg2", "arg3", (char*)0) == -1)
		if(execlp("testinterp.sh","argv1","argv2","argv3",(char*)0)==-1)
			err_sys("execl error\n");
	}
	else {
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error\n");
	}

	exit(EXIT_SUCCESS);
}

/*

1、此时执行的pathname是指向一个二进制可执行文件，则新进程的命令行参数是execl()的"arg1", "arg2", "arg3"：

yexinhai@localhost ~/m/chap8 (master)> ./testinterp
ARG[0]: arg1
ARG[1]: arg2
ARG[2]: arg3
yexinhai@localhost ~/m/chap8 (master)> vim testinterp.c
yexinhai@localhost ~/m/chap8 (master)> make 2&>/dev/null ;make rmobj>/dev/null

1、此时执行的pathname是指向一个解释器文件，则新进程的命令行参数是execl()的"/home/yexinhai/my-apue/chap8/tell","arg2", "arg3"：

yexinhai@localhost ~/m/chap8 (master)> ./testinterp
ARG[0]: /home/yexinhai/my-apue/chap8/tell
ARG[1]: foo
ARG[2]: /home/yexinhai/my-apue/chap8/testinterp.sh
ARG[3]: arg2
ARG[4]: arg3
yexinhai@localhost ~/m/chap8 (master)>

*/
