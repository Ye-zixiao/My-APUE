#include"../include/MyAPUE.h"
#include<pthread.h>

void cleanup(void* args) {
	printf("cleanup: %s\n", (char*)args);
}

void* thread_func1(void* args) {
	printf("thread 1 start\n");
	pthread_cleanup_push(cleanup, "thread 1 first handler");
	pthread_cleanup_push(cleanup, "thread 1 second handler");
	printf("thread 1 push complete\n");
	if (args)
		return (void*)1;
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);

	printf("thread 1 get here?\n");
	return (void*)1;
}


void* thread_func2(void* args) {
	printf("thread 2 start\n");
	pthread_cleanup_push(cleanup, "thread 2 first handler");
	pthread_cleanup_push(cleanup, "thread 2 second handler");
	printf("thread 2 push complete\n");
	if (args)
		pthread_exit((void*)2);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);

	printf("thread 2 get here?\n");
	pthread_exit((void*)2);
}

void* thread_func3(void* args) {
	printf("thread 3 start\n");
	pthread_cleanup_push(cleanup, "thread 3 first handler");
	pthread_cleanup_push(cleanup, "thread 3 second handler");
	printf("thread 3 push complete\n");
	sleep(1);
	if (args)
		pthread_exit((void*)3);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);

	printf("thread 3 get here?\n");
	pthread_exit((void*)3);
}


int main(void)
{
	pthread_t thread1, thread2, thread3;
	void* thread_ret;
	int err;

	if ((err = pthread_create(&thread1, NULL, thread_func1, (void*)1)) != 0)
		err_exit(err, "pthread_create error");
	//if ((err = pthread_create(&thread2, NULL, thread_func2, (void*)2)) != 0)
	//由于该线程最后时刻调用了pthread_cleanup_pop(0),所以该线程不会执行线程清理处理程序
	if ((err = pthread_create(&thread2, NULL, thread_func2, NULL)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_create(&thread3, NULL, thread_func3, (void*)3)) != 0)
		err_exit(err, "pthread_create error");

	//若向某线程发送取消请求，也会导致该线程执行cleanup线程清理程序
	if ((err = pthread_cancel(thread3)) != 0)
		err_exit(err, "pthread_cancel error");
	if ((err = pthread_join(thread1, &thread_ret)) != 0)
		err_exit(err, "pthread_join error");
	printf("thread 1 exit code: %ld\n", (long)thread_ret);
	if ((err = pthread_join(thread2, &thread_ret)) != 0)
		err_exit(err, "pthread_join error");
	printf("thread 2 exit code: %ld\n",(long)thread_ret);
	if ((err = pthread_join(thread3, &thread_ret)) != 0)
		err_exit(err, "pthread_join error");
	printf("thread 3 exit code: %ld\n",
		thread_ret == PTHREAD_CANCELED ? -1 : (long)thread_ret);

	exit(EXIT_SUCCESS);
}
