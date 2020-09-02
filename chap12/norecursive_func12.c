/*
	课本中提到的不适用递归互斥量来解决并发问题
*/

#include"../include/MyAPUE.h"

typedef struct X {
	pthread_mutex_t lock;
	void* saved;
	int value;
}X;


int Xinit(X* x, pthread_mutexattr_t* mutexattr) {
	x->value = 0;
	x->saved = NULL;
	return pthread_mutex_init(&x->lock, mutexattr);
}


void Xdestroy(X* x) {
	pthread_mutex_destroy(&x->lock);
}


//该函数可以由func1调用，它的功能与原先的func2相同，但是我们并不在进入的时候
//对其执行加锁，离开的时候执行解锁操作，这样保证了线程进入时总是以持有互斥量的
//状态进入，但不需要重复加锁或者采用递归互斥量
void func2_locked(X* x) {
	x->value++;
}


void func2(X* x) {
	pthread_mutex_lock(&x->lock);
	x->value++;
	pthread_mutex_unlock(&x->lock);
}


void func1(X* x) {
	pthread_mutex_lock(&x->lock);
	x->value++;
	func2_locked(x);
	pthread_mutex_unlock(&x->lock);
}


void* thread_func(void* args) {
	X* x = (X*)args;

	printf("Thread(0x%lx) begin\n", pthread_self());
	func1(x);
	printf("Thread(0x%lx) end\n", pthread_self());
	pthread_exit((void*)NULL);
}

int main(void)
{
	pthread_t tid;
	int err;
	X* x;

	if ((x = malloc(sizeof(X))) == NULL)
		return -1;
	if ((err = Xinit(x, NULL)) != 0) {
		free(x);
		err_exit(err, "Xinit error");
	}

	Pthread_create(&tid, NULL, thread_func, (void*)x);
	Pthread_join(tid, NULL);
	printf("final value in x: %d\n", x->value);

	Xdestroy(x);
	free(x);
	exit(EXIT_SUCCESS);
}
