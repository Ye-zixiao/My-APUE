#include"MyAPUE.h"
#include<sys/times.h>

static void	
pr_time(clock_t real, const struct tms* pstart, const struct tms* pend) {
	static long clktck;
	if (clktck == 0)
		if ((clktck = sysconf(_SC_CLK_TCK)) < 0)
			err_sys("sysconf error\n");

	printf("\n________________________________________\n");
	printf(" real : %7.3fs\n", real / (double)clktck);
	printf(" user : %7.3fs\n",
		(pend->tms_cutime - pstart->tms_cutime) / (double)clktck);
	printf(" sys  : %7.3fs\n",
		(pend->tms_cstime - pstart->tms_cstime) / (double)clktck);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage: time <execute-file>\n");

	pid_t pid;
	clock_t start, end;
	struct tms starttms, endtms;

	if ((start = times(&starttms)) == -1)
		err_sys("times error\n");
	//任何时刻只要自己没有子进程确实不会记录tms_cxtime
	//printf("start      : %7.3fs\n", start / (double)sysconf(_SC_CLK_TCK));
	//printf("child user : %7.3fs\n", starttms.tms_cutime / (double)sysconf(_SC_CLK_TCK));
	//printf("child sys  : %7.3fs\n", starttms.tms_cstime / (double)sysconf(_SC_CLK_TCK));
	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		if (execvp(argv[1], &argv[1]) == -1) {
			fputs("execvp error\n", stderr);
			_exit(1);
		}
	}
	else {
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error\n");
		/*这样计算real时间确实不是很精确，因为它可能还会计算进除了
		 * 	运行子进程之外的父进程一些运行时间*/
		if ((end = times(&endtms)) == -1)
			err_sys("times error\n");
		pr_time(end - start, &starttms, &endtms);
	}
	
	exit(EXIT_SUCCESS);
}
