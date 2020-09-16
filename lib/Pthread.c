#include"../include/MyAPUE.h"
#include<string.h>

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


/**
 * 创建可分离状态线程
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


/**
 * 使用线程特定数据机制与信号屏蔽机制实现的可重入函数
 * （具体是否是异步信号安全函数有待考量）
 */
#define MAX 1024

extern char** environ;

static pthread_key_t key;
static pthread_mutex_t lock;
static pthread_once_t once_init = PTHREAD_ONCE_INIT;

void thread_init(void) {
	pthread_mutexattr_t mutexattr;

	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&lock, &mutexattr);
	pthread_key_create(&key, free);
	pthread_mutexattr_destroy(&mutexattr);
	//那么问题来了：我们需不需要对互斥量进行解锁
}

char* getenv_r(const char* name) {
	sigset_t oldmask, newmask;
	char* envbuf;
	size_t len;

	sigfillset(&newmask);
	sigdelset(&newmask, SIGSTOP);
	sigdelset(&newmask, SIGKILL);
	if (pthread_sigmask(SIG_SETMASK, &newmask, &oldmask) != 0)
		return NULL;

	len = strlen(name);
	pthread_once(&once_init, thread_init);
	if ((envbuf = (char*)pthread_getspecific(key)) == NULL) {
		if ((envbuf = (char*)malloc(sizeof(char) * MAX)) == NULL) {
			pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
			pthread_mutex_unlock(&lock);
			return NULL;
		}
		pthread_setspecific(key, envbuf);
	}

	for (int i = 0; environ[i] != NULL; ++i) {
		if (strncmp(name, environ[i], len) == 0 &&
			environ[i][len] == '=') {
			strcpy(envbuf, &environ[i][len + 1]);
			pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
			pthread_mutex_unlock(&lock);
			return envbuf;
		}
	}

	pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
	pthread_mutex_unlock(&lock);
	return NULL;
}
