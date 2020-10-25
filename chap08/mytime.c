#include"MyAPUE.h"
#include<time.h>
#include<sys/time.h>
#include<sys/wait.h>


int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage:time <execute-file> <args...>\n");

	pid_t pid;
	struct timeval begin, end;

	gettimeofday(&begin, NULL);
	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		/*子进程的命令行参数是从0这个位置开始，竟然被我设置成execvp(argv[1], &argv[2])，
		  记住：可执行文件名字本身也是参数😡*/
		if (execvp(argv[1], &argv[1]) != -1)
			err_sys("execvp error\n");
	}
	else {
		if (wait(NULL) != pid)
			err_sys("wait error\n");
		gettimeofday(&end, NULL);
		fputs("\n------------------------------\n",stdout);
		printf("total time: %ld us\n",
			(end.tv_sec - end.tv_sec) * 1000000+(end.tv_usec-begin.tv_usec));
	}

	exit(EXIT_SUCCESS);
}

/*
Example:

yexinhai@localhost ~/p/APUE> ./main date +%Y-%m-%d
2020-08-01

------------------------------
total time: 2349 us
yexinhai@localhost ~/p/APUE>
*/
