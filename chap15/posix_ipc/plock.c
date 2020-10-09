#include "../../include/MyAPUE.h"
#include <limits.h>

#define NLOOPS 100000

typedef sem_t plock_t;

plock_t lock;
static volatile int svalue;


void* thread_func(void* args) {
	for (int i = 0; i < NLOOPS; ++i) {
		if (PBSem_Lock(&lock) == -1)
			err_sys("thread(0x%lx) s_lock error", pthread_self());
		svalue++;
		if (PBSem_Unlock(&lock) == -1)
			err_sys("thread(0x%lx) s_unlock error", pthread_self());
	}
	pthread_exit((void*)NULL);
}


int main(void)
{
	pthread_t tid1, tid2;

	if (PBSem_Init(&lock, 0) == -1)
		err_sys("PBSem_Init error");
	if (pthread_create(&tid1, NULL, thread_func, (void*)NULL) == -1)
		err_sys("pthread_create error");
	if (pthread_create(&tid2, NULL, thread_func, (void*)NULL) == -1)
		err_sys("pthread_create error");
	if (pthread_join(tid1, NULL) == -1)
		err_sys("pthread_join error");
	if (pthread_join(tid2, NULL) == -1)
		err_sys("pthread_join error");
	printf("final svalue: %d\n", svalue);
	if (PBSem_Destroy(&lock) == -1)
		err_sys("PBSem_Destroy error");

	exit(EXIT_SUCCESS);
}
