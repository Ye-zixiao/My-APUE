#include"../include/MyAPUE.h"
#include<pthread.h>

typedef struct foo {
	int f_count;
	int f_id;
	int share_value;
	pthread_mutex_t lock;
}foo;

foo* foo_alloc(int id) {
	foo* pret = NULL;

	if ((pret = malloc(sizeof(foo))) != NULL) {
		pret->f_count = 1;
		pret->f_id = id;
		pret->share_value = 0;
		if (pthread_mutex_init(&pret->lock,NULL) != 0) {
			free(pret);
			return NULL;
		}
	}
	return pret;
}

void foo_hold(foo* p) {
	pthread_mutex_lock(&p->lock);
	p->f_count++;
	pthread_mutex_unlock(&p->lock);
}

void foo_update(foo* p) {
	pthread_mutex_lock(&p->lock);
	p->share_value++;
	pthread_mutex_unlock(&p->lock);
}

int foo_read(foo* p) {
	int ret;

	pthread_mutex_lock(&p->lock);
	ret = p->share_value;
	pthread_mutex_unlock(&p->lock);
	return ret;
}

void foo_rele(foo* p) {
	pthread_mutex_lock(&p->lock);
	if (--p->f_count == 0) {
		pthread_mutex_unlock(&p->lock);
		pthread_mutex_destroy(&p->lock);
		free(p);
		printf("foo freed\n");
	}
	else {
		pthread_mutex_unlock(&p->lock);
	}
}

void* thread_func(void* args) {
	foo* p = (foo*)args;
	foo_hold(p);
	for (int i = 0; i < 10000; i++)
		foo_update(p);
	foo_rele(p);
	printf("At the end of thread, the shared value: %d\n", foo_read(p));
	pthread_exit((void*)NULL);
}

int main(void)
{
	pthread_t tid1, tid2, tid3;
	foo* pf;
	int err;

	if ((pf = foo_alloc(1)) == NULL)
		err_sys("foo_alloc error\n");
	if ((err = pthread_create(&tid1, NULL, thread_func, (void*)pf)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_create(&tid2, NULL, thread_func, (void*)pf)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_create(&tid3, NULL, thread_func, (void*)pf)) != 0)
		err_exit(err, "pthread_create error");

	if ((err = pthread_join(tid1, NULL)) != 0)
		err_exit(err, "pthread_join error");
	if ((err = pthread_join(tid2, NULL)) != 0)
		err_exit(err, "pthread_join error");
	if ((err = pthread_join(tid3, NULL)) != 0)
		err_exit(err, "pthread_join error");
	printf("Result shared value: %d\n", foo_read(pf));

	foo_rele(pf);
	exit(EXIT_SUCCESS);
}
