#include"../include/MyAPUE.h"
#include<pthread.h>

void* thread_func(void* args) {
	Sleep(1);
	printf("thread get here?\n");
	pthread_exit((void*)NULL);
}

int main(void)
{
	pthread_t thread_id;
	void* thread_ret;
	int err;

	if ((err = pthread_create(&thread_id, NULL, thread_func, NULL))
		!= 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_cancel(thread_id)) != 0)
		err_exit(err, "pthread_cancel");
	if ((err = pthread_join(thread_id, &thread_ret)) != 0)
		err_exit(err, "pthread_join error");
	if (PTHREAD_CANCELED == thread_ret)
		printf("thread is canceled\n");

	exit(EXIT_SUCCESS);
}
