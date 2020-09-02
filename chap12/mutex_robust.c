#include"../include/MyAPUE.h"
#include<limits.h>

pthread_mutex_t lock;

void* thread1(void* args) {
	printf("thread(0x%lx) begin\n", pthread_self());
	pthread_mutex_lock(&lock);
	sleep(1);
	printf("thread(0x%lx) end\n", pthread_self());

	pthread_exit((void*)NULL);
}

void* thread2(void* args) {
	int err;

	printf("thread(0x%lx) begin\n", pthread_self());
	if((err=pthread_mutex_lock(&lock))!=0)
		if (err == EOWNERDEAD) {
			/*
				若该线程在调用pthread_mutex_unlock函数之前不执行pthread_mutex_consistent
				互斥量一致函数，则该线程根本不可能获得该互斥量的持有权
			*/
			if ((err = pthread_mutex_consistent(&lock)) != 0)
				pthread_exit((void*)-1);
			if ((err = pthread_mutex_unlock(&lock) != 0))
				pthread_exit((void*)-2);
			if ((err = pthread_mutex_lock(&lock)) != 0)//试图重新加锁
				pthread_exit((void*)-3);
		}
	printf("thread(0x%lx) get mutex\n", pthread_self());
	sleep(1);
	pthread_mutex_unlock(&lock);
	printf("thread(0x%lx) end\n", pthread_self());

	pthread_exit((void*)NULL);
}

int main(void)
{
	pthread_mutexattr_t mutexattr;
	pthread_t tid1, tid2;
	int err;
	
	if ((err = pthread_mutexattr_init(&mutexattr)) != 0)
		err_exit(err, "pthread_mutexattr_init error");
	if ((err = pthread_mutexattr_setrobust(&mutexattr, PTHREAD_MUTEX_ROBUST)) == 0) {
		if ((err = pthread_mutex_init(&lock, &mutexattr)) != 0)
			err_exit(err, "pthread_mutex_init error");
		Pthread_create(&tid1, NULL, thread1, (void*)NULL);
		sleep(1);	//确保tid1线程先得到调度，并在加锁互斥量的状态下终止线程
		Pthread_create(&tid2, NULL, thread2, (void*)NULL);
		Pthread_join(tid1, NULL);
		Pthread_join(tid2, NULL);
		pthread_mutex_destroy(&lock);
	}
	pr_mutexattr(&mutexattr);
	pthread_mutexattr_destroy(&mutexattr);

	exit(EXIT_SUCCESS);
}
