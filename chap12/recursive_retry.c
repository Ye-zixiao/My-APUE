#include"../include/MyAPUE.h"

typedef struct to_info {
	void (*to_func)(void*);
	void* to_args;
	struct timespec when;
}to_info;


pthread_mutex_t lock;
pthread_mutexattr_t mutexattr;


/*
	超时后线程执行的函数例程
*/
void* timeout_helper(void* args) {
	to_info* tip = (to_info*)args;

	printf("timeout_helper begin\n");
	clock_nanosleep(CLOCK_REALTIME, 0, &tip->when, NULL);
	tip->to_func(tip->to_args);
	free(args);
	printf("timeout_helper end\n");

	pthread_exit((void*)NULL);
}


/*
	定时设定函数
*/
void timeout(const struct timespec* when, void (*pf)(void*), void* args) {
	to_info* tip;
	int err;

	if (when->tv_sec > 0 || when->tv_nsec > 0) {
		if ((tip = malloc(sizeof(to_info))) != NULL) {
			tip->to_func = pf;
			tip->to_args = args;
			tip->when.tv_sec = when->tv_sec;
			tip->when.tv_nsec = when->tv_nsec;

			if ((err = makeDetachedThread(timeout_helper, (void*)tip)) != 0)
				free(tip);
			else return;
		}
	}
	pf(args);
}


/*
	超时后执行的函数操作	
*/
void retry(void* args) {
	pthread_mutex_lock(&lock);
	/*
		do sth
	*/
	printf("value of args: %d\n", (*(int*)args)++);

	pthread_mutex_unlock(&lock);
}


int main(void)
{
	int err, condition, args;
	struct timespec when;

	if ((err = pthread_mutexattr_init(&mutexattr)) != 0)
		err_exit(err, "pthread_mutexattr_init error");
	if ((err = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE)) != 0)
		err_exit(err, "pthread_mutexattr_settype error");
	if ((err = pthread_mutex_init(&lock, &mutexattr)) != 0)
		err_exit(err, "pthread_mutex_init error");

	condition = 1;
	args = 233;
	pthread_mutex_lock(&lock);
	if (condition) {
		when.tv_sec = 5; when.tv_nsec = 0;
		timeout(&when, retry, (void*)&args);
	}
	pthread_mutex_unlock(&lock);

	/*
		some process...
	*/
	printf("main thread is still running...\n");

	sleep(6);
	printf("result value of args: %d\n", args);
	pthread_mutexattr_destroy(&mutexattr);
	pthread_mutex_destroy(&lock);
	exit(EXIT_SUCCESS);
}
