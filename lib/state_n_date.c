#include "../include/MyAPUE.h"
#include <string.h>
#include <limits.h>
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


#define TIMESTRLEN 32

/**
 * 非线程安全地获取当前的时间
 */
const char *currTime(void) {
	static char buf[TIMESTRLEN];
	struct timeval timevalbuf;
	struct tm* ptm;

	gettimeofday(&timevalbuf, NULL);
	if ((ptm = localtime(&timevalbuf.tv_sec)) == NULL)
		return NULL;
	strftime(buf, TIMESTRLEN, "%H:%M:%S", ptm);
	return buf;
}


/*
	用来帮助pthread_cond_timewait这样的函数生成绝对的定时时间
*/
void get_abstime(struct timespec* tsp, long seconds) {
	if (clock_gettime(CLOCK_REALTIME, tsp) != 0)
		err_sys("clock_gettime error\n");
	tsp->tv_sec += seconds;
}


/*
	一种非线程安全获取环境变量值的getenv函数实现
*/
static char envbuf[BUFSIZE];
extern char** environ;

char* mygetenv(const char* name) {
	size_t len = strlen(name);

	for (int i = 0; environ[i] != NULL; ++i) {
		if (strncmp(environ[i], name, len) == 0 &&
			environ[i][len] == '=') {
			strncpy(envbuf, &environ[i][len + 1], BUFSIZE);
			return envbuf;
		}
	}
	return NULL;
}


/*
	具有线程安全性且异步信号安全的getenv函数实现
*/
pthread_mutex_t env_mutex;

static pthread_once_t init_done = PTHREAD_ONCE_INIT;

static void env_mutexinit(void) {
	pthread_mutexattr_t mutexattr;

	pthread_mutexattr_init(&mutexattr);
	//这里不适用递归锁，也可以保证mygetenv_r函数具有线程安全性，但则并不能保证异步信号安全
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&env_mutex, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);
}

int mygetenv_r(const char* name, char* buf, int buflen) {
	int len, keylen;

	len = strlen(name);
	//只有最初执行的线程会对执行env_mutexinit例程，后面的线程不会执行之
	pthread_once(&init_done, env_mutexinit);
	pthread_mutex_lock(&env_mutex);
	for (int i = 0; environ[i] != NULL; ++i) {
		if (strncmp(name, environ[i], len) == 0 && environ[i][len] == '=') {
			keylen = strlen(&environ[i][len + 1]);
			if (keylen >= buflen) {
				pthread_mutex_unlock(&env_mutex);
				return ENOSPC;
			}
			else {
				strncpy(buf, &environ[i][len + 1], buflen);
				pthread_mutex_unlock(&env_mutex);
				return 0;
			}
		}
	}
	pthread_mutex_unlock(&env_mutex);
	return ENOENT;
}


/*
	打印互斥量属性对象中记录的属性
*/
int pr_mutexattr(const pthread_mutexattr_t* mutexattr) {
	static const char* typestr[] = {
		"PTHREAD_MUTEX_NORMAL",
		"PTHREAD_MUTEX_RECURSIVE",
		"PTHREAD_MUTEX_ERRORCHECK",
		"OTHER"
	};
	int pshared, robust, type, err;

	if ((err = pthread_mutexattr_getpshared(mutexattr, &pshared)) != 0)
		return err;
	if ((err = pthread_mutexattr_getrobust(mutexattr, &robust)) != 0)
		return err;
	if ((err = pthread_mutexattr_gettype(mutexattr, &type)) != 0)
		return err;
	printf("mutex pshared: %s\n", pshared == PTHREAD_PROCESS_SHARED ?
		"PTHREAD_PROCESS_SHARED" : "PTHREAD_PROCESS_PRIVATE");
	printf("mutex robust : %s\n", robust == PTHREAD_MUTEX_ROBUST ?
		"PTHREAD_MUTEX_ROBUST" : "PTHREAD_MUTEX_STALLED");
	printf("mutex type   : %s\n", typestr[type > 3 ? 3 : type]);

	return err;
}


/**
 * 返回最大可打开文件描述符值
 */
#ifdef OPEN_MAX
long openmax = OPEN_MAX;
#else
long openmax = 0;
#endif

#define OPEN_MAX_GUESS 256

long open_max(void) {
	if (openmax == 0) {
		errno = 0;
		if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
			//若OPEN_MAX值无法确定，则errno不改变
			if (errno == 0)
				openmax = OPEN_MAX_GUESS;
			else
				err_sys("sysconf error for _SC_OPEN_MAX");
		}
	}
	return openmax;
}
