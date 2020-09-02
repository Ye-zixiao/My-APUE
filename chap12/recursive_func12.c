/*
	书本中提到的使用递归互斥量解决并发问题
*/

#include"../include/MyAPUE.h"

typedef struct X {
	pthread_mutex_t lock;
	void* saved;
	int value;
}X;

int Xinit(X* x, pthread_mutexattr_t* attr) {
	int err;

	x->value = 0;
	x->saved = (void*)NULL;
	err = pthread_mutex_init(&x->lock, attr);
	return err;
}


void Xdestroy(X* x) {
	pthread_mutex_destroy(&x->lock);
}

void func2(X* x) {
	pthread_mutex_lock(&x->lock);
	x->value++;
	pthread_mutex_unlock(&x->lock);
}


void func1(X* x) {
	pthread_mutex_lock(&x->lock);
	x->value++;
	func2(x);
	pthread_mutex_unlock(&x->lock);
}


void* thread_func(void* args) {
	X* x = (X*)args;

	printf("Thread(0x%lx) begin\n", pthread_self());
	func1(x);
	sleep(1);
	printf("Thread(0x%lx) end\n", pthread_self());
	pthread_exit((void*)NULL);
}

int main(void)
{
	pthread_mutexattr_t mutexattr;
	pthread_t tid;
	int err;
	X* x;

	//预处理互斥量属性和互斥量
	if ((x = malloc(sizeof(X))) == NULL)
		return -1;
	if ((err = pthread_mutexattr_init(&mutexattr)) != 0) {
		free(x);
		err_exit(err, "pthread_mutex_init error");
	}
	if ((err = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE)) != 0) {
		free(x);
		err_exit(err, "pthread_mutexattr_settype error");
	}
	if ((err = Xinit(x, &mutexattr)) != 0) {
		free(x);
		err_exit(err, "Xinit error");
	}

	//创建线程并执行回收
	Pthread_create(&tid, NULL, thread_func, (void*)x);
	Pthread_join(tid, NULL);
	printf("final value in x: %d\n", x->value);
	
	//尾处理
	Xdestroy(x);
	pthread_mutexattr_destroy(&mutexattr);
	free(x);
	exit(EXIT_SUCCESS);
}
