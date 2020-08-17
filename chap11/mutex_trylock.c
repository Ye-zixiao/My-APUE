#include"../include/MyAPUE.h"
#include<pthread.h>

typedef struct foo {
	pthread_mutex_t first_lock;
	pthread_mutex_t second_lock;
	int value;
}foo;

foo* foo_init(void) {
	foo* pret = NULL;

	if ((pret = malloc(sizeof(foo))) != NULL) {
		pret->value = 0;
		if (pthread_mutex_init(&pret->first_lock, NULL) != 0) {
			free(pret);
			return NULL;
		}
		if (pthread_mutex_init(&pret->second_lock, NULL) != 0) {
			free(pret);
			return NULL;
		}
	}
	return pret;
}

void foo_destroy(foo* p) {
	if (p == NULL)return;
	pthread_mutex_destroy(&p->first_lock);
	pthread_mutex_destroy(&p->second_lock);
	free(p);
}

void update_foo1(foo* p) {
	while (1) {
		pthread_mutex_lock(&p->first_lock);
		if (pthread_mutex_trylock(&p->second_lock) == EBUSY) {
			pthread_mutex_unlock(&p->first_lock);
			continue;
		}
		pthread_mutex_unlock(&p->first_lock);
		p->value++;
		pthread_mutex_unlock(&p->second_lock);
		break;
	}
}

void update_foo2(foo* p) {
	while (1) {
		pthread_mutex_lock(&p->second_lock);
		if (pthread_mutex_trylock(&p->first_lock) == EBUSY) {
			pthread_mutex_unlock(&p->second_lock);
			continue;
		}
		pthread_mutex_unlock(&p->second_lock);
		p->value++;
		pthread_mutex_unlock(&p->first_lock);
		break;
	}
}

void* thread1_func(void* args) {
	foo* p = (foo*)args;
	for (int i = 0; i < 100000; i++)
		update_foo1(p);
	pthread_exit((void*)NULL);
}

void* thread2_func(void* args) {
	foo* p = (foo*)args;
	for (int i = 0; i < 100000; i++)
		update_foo2(p);
	pthread_exit((void*)NULL);
}

int main(void)
{
	pthread_t thr1, thr2;
	foo* p;
	int err;

	if ((p = foo_init()) == NULL)
		err_sys("foo_init error\n");
	if ((err = pthread_create(&thr1, NULL, thread1_func, (void*)p)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_create(&thr2, NULL, thread2_func, (void*)p)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_join(thr1, NULL)) != 0)
		err_exit(err, "pthread_join error");
	if ((err = pthread_join(thr2, NULL)) != 0)
		err_exit(err, "pthread_join error");
	printf("Result shared_value = %d\n", p->value);

	foo_destroy(p);
	exit(EXIT_SUCCESS);
}
