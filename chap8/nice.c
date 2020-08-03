/*
	由于自己的测试平台CPU为2C4T，所以无法看出运行在两个核心上的进程之间因nice改变而造成的区别
*/
#include"apue.h"
#include<sys/time.h>
#include<limits.h>
#include<errno.h>

unsigned long long count;
struct timeval end;

void checktime(const char* str) {
	struct timeval tv;

	gettimeofday(&tv, NULL);
	if (tv.tv_sec >= end.tv_sec && tv.tv_usec >= end.tv_usec) {
		printf("%s count = %lld\n", str, count);
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char* argv[])
{
	pid_t pid;
	char* s;
	int nzero, adj = 0;

	setbuf(stdout, NULL);
	nzero = sysconf(_SC_NZERO);
	printf("NZERO = %d\n", nzero);
	if (argc == 2)
		adj = strtol(argv[1], NULL, 10);
	gettimeofday(&end, NULL);
	end.tv_sec += 10;

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		s = "child";
		printf("current nice value in child is %d, adjusting by %d\n",
			nice(0) + nzero, adj);
		errno = 0;
		if (nice(adj) == -1 && errno != 0)
			err_sys("child set scheduling priority");//若nice返回-1且errno!=0说明真的出错
		printf("now child nice value = %d\n", nice(0) + nzero);
	}
	else {
		s = "parent";
		printf("current nice value in parent is %d\n", nice(0) + nzero);
	}
	while (1) {
		if (++count == 0)
			err_sys("%s counter wrap", s);
		checktime(s);
	}
	printf("nice value = %d\n", nice(0) + NZERO);
}