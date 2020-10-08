#include "../include/MyAPUE.h"
#include <semaphore.h>

#define NLOOPS 1000000

typedef sem_t plock_t;

int plock_init(plock_t* lock, int pshared) {
	return sem_init(lock, pshared, 1);
}


int plock(plock_t* lock) {
	return sem_wait(lock);
}


int punlock(plock_t* lock) {
	return sem_post(lock);
}

int plock_destroy(plock_t* lock) {
	return sem_destroy(lock);
}

static volatile int value;
plock_t lock;

void* thread_func(void* args) {
	for (int i = 0; i < NLOOPS; ++i) {
		if (plock(&lock) == -1)
			err_sys("thread 0x%lx plock error", pthread_self());
		++value;
		if (punlock(&lock) == -1)
			err_sys("thread 0x%lx punlock error", pthread_self());
	}
	pthread_exit((void*)NULL);
}

int main(void)
{
	pthread_t tid1, tid2;
	
	if (plock_init(&lock, 0) == -1)
		err_sys("plock_init error");
	if (pthread_create(&tid1, NULL, thread_func, (void*)NULL) == -1)
		err_sys("pthread_create error");
	if (pthread_create(&tid2, NULL, thread_func, (void*)NULL) == -1)
		err_sys("pthread_create error");
	if (pthread_join(tid1, NULL) == -1)
		err_sys("pthread_join error");
	if (pthread_join(tid2, NULL) == -1)
		err_sys("pthread_join error");
	if (plock_destroy(&lock) == -1)
		err_sys("plock_destroy error");

	printf("final value result: %d\n", value);
	exit(EXIT_SUCCESS);
}
