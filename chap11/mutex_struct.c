#include"../include/MyAPUE.h"
#include<pthread.h>

typedef struct foo {
	int shared_value;
	pthread_mutex_t lock;
}foo;

foo* foo_alloc(void) {
	foo* pret = NULL;

	if ((pret = malloc(sizeof(foo))) != NULL) {
		pret->shared_value = 0;
		if (pthread_mutex_init(&pret->lock, NULL) != 0) {
			free(pret);
			return NULL;
		}
	}
	return pret;
}

void foo_destroy(foo* pfoo) {
	if (pfoo == NULL)return;
	pthread_mutex_destroy(&pfoo->lock);
	free(pfoo);
}

void add_foo(foo* p) {
	pthread_mutex_lock(&p->lock);
#ifdef DISPLAY_
	int val = ++(p->shared_value);
#else
	p->shared_value++;
#endif
	pthread_mutex_unlock(&p->lock);
#ifdef DISPLAY_
	printf("Thread(0x%lx) increase shared_value: %d\n",
		pthread_self(), val);
#endif
}

void* thread_func(void* args) {
	foo* p = (foo*)args;
	for (int i = 0; i < 10000; ++i)
		add_foo(p);
	pthread_exit((void*)NULL);
}


int main(void) {
	pthread_t thr1, thr2;
	foo* pfoo;
	int err;

	if ((pfoo = foo_alloc()) == NULL)
		err_sys("foo_alloc error\n");
	printf("before pthread_create, pfoo->shared_value= %d\n", pfoo->shared_value);
	if ((err = pthread_create(&thr1, NULL, thread_func, (void*)pfoo)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_create(&thr2, NULL, thread_func, (void*)pfoo)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_join(thr1, NULL)) != 0)
		err_exit(err, "pthread_join error");
	if ((err = pthread_join(thr2, NULL)) != 0)
		err_exit(err, "pthread_join  error");

	printf("after pthread_join, pfoo->shared_value= %d\n", pfoo->shared_value);
	foo_destroy(pfoo);
	exit(EXIT_SUCCESS);
}
