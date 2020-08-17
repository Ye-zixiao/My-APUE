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
