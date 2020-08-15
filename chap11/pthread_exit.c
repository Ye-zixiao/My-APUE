#include"../include/MyAPUE.h"
#include<pthread.h>

void* thread_func1(void* args) {
	printf("thread1 returning...\n");
	return (void*)1;
}

void* thread_func2(void* args) {
	printf("thread2 returning...\n");
	pthread_exit((void*)2);
}


int main(void)
{
	pthread_t thread_id1, thread_id2;
	void* thread_ret;
	int err;

	if ((err = pthread_create(&thread_id1, NULL, thread_func1, NULL))
		!= 0)
		err_exit(err, "create thread1 error");
	if ((err = pthread_create(&thread_id2, NULL, thread_func2, NULL))
		!= 0)
		err_exit(err, "create thread2 error");

	if ((err = pthread_join(thread_id1, &thread_ret)) != 0)
		err_exit(err, "thread1 join error");
	printf("thread 1 exit code: %ld\n", (long)thread_ret);
	if ((err = pthread_join(thread_id2, &thread_ret)) != 0)
		err_exit(err, "thread2 join error");
	printf("thread 2 exit code: %ld\n", (long)thread_ret);

	exit(EXIT_SUCCESS);
}
