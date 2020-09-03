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


int pr_mutexattr(const pthread_mutexattr_t* mutexattr) {
	static const char* typestr[] = {
		"PTHREAD_MUTEX_NORMAL",
		"PTHREAD_MUTEX_RECURSIVE",
		"PTHREAD_MUTEX_ERRORCHECK",
	};
	int pshared, robust, type, err;

	if ((err = pthread_mutexattr_getpshared(mutexattr, &pshared)) != 0)
		return err;
	if ((err = pthread_mutexattr_getrobust(mutexattr, &robust)) != 0)
		return err;
	if ((err = pthread_mutexattr_gettype(mutexattr, &type)) != 0)
		return err;
	printf("mutex pshared: %s\n", pshared == PTHREAD_PROCESS_SHARED ?
		"PTHREAD_PROCESS_SHARED" : "PTHREAD_PROCESS_PRIVATE");
	printf("mutex robust : %s\n", robust == PTHREAD_MUTEX_ROBUST ?
		"PTHREAD_MUTEX_ROBUST" : "PTHREAD_MUTEX_STALLED");
	printf("mutex type   : %s\n", typestr[type]);

	return err;
}


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
