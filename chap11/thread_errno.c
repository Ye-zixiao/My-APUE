#include"../include/MyAPUE.h"
#include<pthread.h>

void* thread_func(void* args) {
	errno = 10;//每一个线程都有属于自己的errno变量
	printf("in thread_func: errno = %d\n", errno);
	return (void*)NULL;
}


int main(void)
{
	pthread_t thread_id;
	int err;

	errno = 11;
	printf("before pthread_create: errno = %d\n", errno);
	if ((err = pthread_create(&thread_id, NULL, thread_func, NULL))
		!= 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_join(thread_id, NULL)) != 0)
		err_exit(err, "pthread_join error");
	printf("after pthread_join: errno = %d\n", errno);

	exit(EXIT_SUCCESS);
}
