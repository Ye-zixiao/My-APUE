#include"../include/MyAPUE.h"
#include<string.h>
//#define _RESUID

void pr_exit(int status) {
	if (WIFEXITED(status))//正常终止
		printf("normal termination,   exit status   = %d\n",
			WEXITSTATUS(status));
	else if (WIFSIGNALED(status))//异常终止
		printf("abnormal termination, signal number = %d%s\n",
			WTERMSIG(status),
#ifdef WCOREDUMP
			WCOREDUMP(status) ? " (core file generated)" : "");
#else
		"");
#endif
	else if (WIFSTOPPED(status))//进程暂停
		printf("child stopped,        signal number = %d\n",
			WSTOPSIG(status));
}


void _pr_limit(const char* rname, int resource) {
	struct rlimit limitbuf;

	if (getrlimit(resource, &limitbuf) != 0)
		err_sys("getrlimit error\n");
	printf("%s\n", rname);
	if (limitbuf.rlim_cur == RLIM_INFINITY)
		printf("soft limit: unlimited\n");
	else
		printf("soft limit: %lukb\n", limitbuf.rlim_cur / 1024);
	if (limitbuf.rlim_max == RLIM_INFINITY)
		printf("hard limit: unlimited\n");
	else
		printf("hard limit: %lukb\n", limitbuf.rlim_max / 1024);
}


#ifdef _RESUID
void print_resuid(void) {
	uid_t ruid, euid, suid;
	if (getresuid(&ruid, &euid, &suid) == -1)
		err_sys("getresuid error\n");
	printf("ruid: %d, euid: %d, suid: %d\n",
		ruid, euid, suid);
}
#endif


void pr_mask(const char* str) {
	sigset_t sigset;
	int saved_errno;

	saved_errno = errno;
	if (sigprocmask(0, NULL, &sigset) == -1)
		err_sys("sigprocmask error\n");
	else {
		printf("%s ", str);
		if (sigismember(&sigset, SIGINT))
			printf(" SIGINT");
		if (sigismember(&sigset, SIGQUIT))
			printf(" SIGQUIT");
		if (sigismember(&sigset, SIGUSR1))
			printf(" SIGUSR1");
		if (sigismember(&sigset, SIGUSR2))
			printf(" SIGUSR2");
		if (sigismember(&sigset, SIGALRM))
			printf(" SIGALRM");
		printf("\n");
	}
	errno = saved_errno;
}


void pr_now(void) {//打印当前时间
	const int BufSize = 64;
	struct timeval timevalbuf;
	struct tm* ptm;
	char buf[BufSize];

	gettimeofday(&timevalbuf, NULL);
	if ((ptm = localtime(&timevalbuf.tv_sec)) == NULL)
		err_sys("localtime error\n");
	if (strftime(buf, BufSize, "%Y-%m-%d %H:%M:%S", ptm) == 0)
		err_sys("strftime error\n");
	printf("Current time is %s\n", buf);
}


/*
	用来帮助pthread_cond_timewait这样的函数生成绝对的定时时间
*/
void get_abstime(struct timespec* tsp, long seconds) {
	if (clock_gettime(CLOCK_REALTIME, tsp) != 0)
		err_sys("clock_gettime error\n");
	tsp->tv_sec += seconds;
}
