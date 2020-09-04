#include"../include/MyAPUE.h"


void Pthread_create(pthread_t* tid, 
	const pthread_attr_t* attr, void* (*thread_func)(void*), void* args) {
	int err;

	if ((err = pthread_create(tid, attr, thread_func, args)) != 0)
		err_exit(err, "pthread_create error");
}

void Pthread_join(pthread_t tid, void** rval_ptr) {
	int err;

	if ((err = pthread_join(tid, rval_ptr)) != 0)
		err_exit(err, "pthread_join error");
}




/*
	创建可分离状态线程
*/
int makeDetachedThread(void* (*thread_func)(void*), void* args) {
	pthread_attr_t attr;
	pthread_t tid;
	int err;

	if ((err = pthread_attr_init(&attr)) != 0)
		return err;
	if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) == 0)
		err = pthread_create(&tid, &attr, thread_func, args);
	pthread_attr_destroy(&attr);
	return err;
}
