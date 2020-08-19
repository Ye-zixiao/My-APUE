#include"../include/MyAPUE.h"

typedef struct svalue {
	pthread_spinlock_t lock;
	volatile int value;
}svalue;

void svalue_init(svalue* psv) {
	int err;

	if ((err = pthread_spin_init(&psv->lock, PTHREAD_PROCESS_PRIVATE))
		!= 0)
		err_exit(err, "pthread_spin_init error");
	psv->value = 0;
}

void svalue_destroy(svalue* psv) {
	int err;

	if ((err = pthread_spin_destroy(&psv->lock)) != 0)
		err_exit(err, "pthread_spin_destroy error");
}

void* thread_func(void* args) {
	svalue* psv = (svalue*)args;

	for (int i = 0; i < 10000; i++) {
		pthread_spin_lock(&psv->lock);
		psv->value++;
		pthread_spin_unlock(&psv->lock);
	}
	pthread_exit((void*)NULL);
}


int main(void) {
	pthread_t tid1, tid2, tid3;
	svalue* psv;

	if ((psv = malloc(sizeof(svalue))) == NULL)
		err_sys("malloc error\n");
	svalue_init(psv);
	Pthread_create(&tid1, NULL, thread_func, (void*)psv);
	Pthread_create(&tid2, NULL, thread_func, (void*)psv);
	Pthread_create(&tid3, NULL, thread_func, (void*)psv);
	Pthread_join(tid1, NULL);
	Pthread_join(tid2, NULL);
	Pthread_join(tid3, NULL);
	printf("At the end, the value of svalue: %d\n", psv->value);
	svalue_destroy(psv);
	free(psv);

	exit(EXIT_SUCCESS);
}
