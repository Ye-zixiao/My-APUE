#include"../include/MyAPUE.h"
#include<pthread.h>

//#define DEBUG
pthread_t ntid;

void pr_ids(const char* s) {
	printf("%s pid %d tid %lu (0x%lx)\n", s,
		getpid(), pthread_self(), pthread_self());
}

void* thread_func(void* args) {
	pr_ids("new  thread:");
	return (void*)NULL;
}


int main(void)
{
	int err;

#ifdef DEBUG
	printf("before pthread_create ntid: %lu\n",ntid);
#endif
	if ((err = pthread_create(&ntid, NULL, thread_func, NULL)) != 0)
		err_exit(err, "pthread_create error");
	pr_ids("main thread:");
#ifdef DEBUG
	printf("after pthread_create ntid: %lu\n",ntid);
#endif
	sleep(1);
	exit(EXIT_SUCCESS);
}
