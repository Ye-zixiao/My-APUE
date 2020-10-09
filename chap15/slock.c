#include "../include/MyAPUE.h"
#include <semaphore.h>
#include <limits.h>

#define NLOOPS 100000

struct slock {
	sem_t* semp;
	char name[_POSIX_NAME_MAX];
};

struct slock* myslock;
static volatile int svalue;


/**
 * 分配一个二值信号量
 */
struct slock* s_alloc(void) {
	struct slock* sp;
	static int cnt;

	if ((sp = malloc(sizeof(struct slock))) == NULL)
		return NULL;
	do {
		snprintf(sp->name, sizeof(sp->name), "/%ld.%d", (long)getpid(), cnt++);
		sp->semp = sem_open(sp->name, O_CREAT | O_EXCL, S_IRWXU, 1);
	} while ((sp->semp == SEM_FAILED) && (errno = EEXIST));
	if (sp->semp == SEM_FAILED) {
		free(sp);
		return NULL;
	}
	/* 成功之后立即断开与信号量的连接，删除名字，防止其他
		进程对其的引用，简化进程终止后的清理工作 */
	sem_unlink(sp->name);
	return sp;
}

/**
 * 释放信号量相关的资源
 */
void s_free(struct slock* sp) {
	sem_close(sp->semp);
	free(sp);
}


int s_lock(struct slock* sp) {
	return sem_wait(sp->semp);
}


int s_unlock(struct slock* sp) {
	return sem_post(sp->semp);
}


void* thread_func(void* args) {
	for (int i = 0; i < NLOOPS; ++i) {
		if (s_lock(myslock) == -1)
			err_sys("thread(0x%lx) s_lock error", pthread_self());
		svalue++;
		if (s_unlock(myslock) == -1)
			err_sys("thread(0x%lx) s_unlock error", pthread_self());
	}
	pthread_exit((void*)NULL);
}


int main(void)
{
	pthread_t tid1, tid2;

	if ((myslock = s_alloc()) == NULL)
		err_sys("s_alloc error");
	if (pthread_create(&tid1, NULL, thread_func, (void*)NULL) == -1)
		err_sys("pthread_create error");
	if (pthread_create(&tid2, NULL, thread_func, (void*)NULL) == -1)
		err_sys("pthread_create error");
	if (pthread_join(tid1, NULL) == -1)
		err_sys("pthread_join error");
	if (pthread_join(tid2, NULL) == -1)
		err_sys("pthread_join error");
	printf("final svalue: %d\n", svalue);
	s_free(myslock);

	exit(EXIT_SUCCESS);
}
