#include"apue.h"

int globalvalue = 6;
char buf[] = "a write to stdout\n";


int main(void)
{
	int var = 88;
	pid_t pid;

	if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1)
		err_sys("write error");
	printf("before fork\n");
		/*由于标准I/O的输出数据在未刷新的时存放在用户进程空间的输出流缓冲区中，
		所以一旦fork之后子进程的进程空间中复制出来的流缓冲区中也会有这些数据，
		所以fork之后子进程和父进程都会各输出相同的数据一次。而系统调用write
		就不会这样*/
	//fflush(stdout);//若fflush刷新标准输出就不会在重定向到文件的时候输出两次

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		globalvalue++;
		var++;
	}
	else
		sleep(2);

	printf("pid =%ld, globalvalue =%d, var =%d\n", (long)getpid(),
		globalvalue, var);
	exit(EXIT_SUCCESS);
}