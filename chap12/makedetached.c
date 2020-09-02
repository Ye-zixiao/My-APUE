#include"../include/MyAPUE.h"

void pr_threadattr(const pthread_attr_t* pattr) {
	int detachstate, err;
	size_t guardsize;
	size_t stacksize;
	
	if ((err = pthread_attr_getdetachstate(pattr, &detachstate)) != 0)
		return;
	if ((err = pthread_attr_getguardsize(pattr, &guardsize)) != 0)
		return;
	if ((err = pthread_attr_getstacksize(pattr, &stacksize)) != 0)
		return;
	printf("Thread state: %s\n", detachstate == PTHREAD_CREATE_DETACHED ?
		"Detached" : "Joinable");
	printf("Thread guardsize: %ld\n", guardsize);
	printf("Thread stacksize: %ld\n", stacksize);
}

int makeThread(void* (*func)(void*), void* args) {
	pthread_attr_t attr;
	pthread_t thd;
	int err;

	if ((err = pthread_attr_init(&attr)) != 0)
		return err;
	if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) == 0)
		if ((err = pthread_attr_setstacksize(&attr, 40960)) == 0)
			err = pthread_create(&thd, &attr, func, args);
	pr_threadattr(&attr);
	pthread_attr_destroy(&attr);
	return err;
}

void* thread_func(void* args) {
	printf("Thread(0x%lx) begin\n", pthread_self());
	sleep(1);
	printf("Thread(0x%lx) end\n", pthread_self());
	pthread_exit((void*)NULL);
}

int main(void)
{
	int err;

	printf("Main thread begin\n");
	if((err=makeThread(thread_func,(void*)NULL))!=0)
		err_exit(err,"makeThread error");
	sleep(2);
	printf("Main thread end\n");

	exit(EXIT_SUCCESS);
}
