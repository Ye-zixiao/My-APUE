#include"../include/MyAPUE.h"

pthread_mutex_t lock;
pthread_mutexattr_t mutexattr;

void* thread_func1(void* args) {
	pthread_mutex_lock(&lock);
	printf("thread(0x%lx) locks mutex but doesn't unlock it\n",
		pthread_self());
	return (void*)NULL;
}

void* thread_func2(void* args) {
	int err;

	while ((err = pthread_mutex_lock(&lock)) != 0) {
		if (err == EOWNERDEAD) {
			/* 若先不执行pthread_mutex_consistent函数，则当前线程是无法对
				一个死亡线程持有的健壮互斥量进行恢复*/
			if (pthread_mutex_consistent(&lock) != 0)
				err_sys("pthread_mutex_consistent error\n");
			if (pthread_mutex_unlock(&lock) != 0)
				err_sys("pthread_mutex_unlock error\n");
			printf("thread(0x%lx) restore the lock\n", pthread_self());
		}
		else
			err_sys("pthread_mutex_lock error\n");
	}
	printf("thread(0x%lx) get in critical zone\n", pthread_self());
	pthread_mutex_unlock(&lock);
	return (void*)NULL;
}


int main(void)
{
	int err;

	if ((err = pthread_mutexattr_init(&mutexattr)) != 0)
		err_exit(err, "pthread_mutexattr_init error");
	if ((err = pthread_mutexattr_setrobust(&mutexattr, PTHREAD_MUTEX_ROBUST)) != 0)
		err_exit(err, "pthread_mutex_setrobust error");
	if ((err = pthread_mutex_init(&lock, &mutexattr)) != 0)
		err_exit(err, "pthread_mutex_init error");

	makeDetachedThread(thread_func1, (void*)NULL);
	makeDetachedThread(thread_func2, (void*)NULL);

	sleep(1);
	exit(EXIT_SUCCESS);
}
