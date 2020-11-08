/**
 * 使用System V（XSI）Semaphore实现的二值信号量
 */
#include "MyAPUE.h"
#include <sys/ipc.h>
#include <sys/sem.h>


#define DEFAULT_PATH "/tmp"
#define DEFAULT_PROID 1

union semun {
	int val;
	struct semid_ds* buf;
	unsigned short* array;
};


/**
 * 创建或引用一个二值信号量，然后对其调用semop函数设置其值为1，改变
 * 与信号量相关的semid_ds中的sem_otime，防止竞态条件的发生。在创建
 * 信号量进程还没有改变sem_time之前别的进程是无法对该信号量进行操作的
 */
int BSem_Create(lock_t* lock, const char* path, int projid) {
	key_t semkey;
	union semun arg;
	struct sembuf sbuf;
	struct semid_ds dsbuf;
	const int MAX_TRIES = 10;

	if (path == NULL || projid < 0 || projid>255) {
		path = DEFAULT_PATH;
		projid = DEFAULT_PROID;
	}
	if ((semkey = ftok(path, projid)) == -1)
		return -1;
	if ((*lock = semget(semkey, 1, IPC_CREAT | IPC_EXCL | IPC_PERM)) != -1) {
		arg.val = 0;
		sbuf.sem_num = 0;
		sbuf.sem_op = 1;
		sbuf.sem_op = 0;

		if (semctl(*lock, 0, SETVAL, arg) == -1)
			return -1;
		if (semop(*lock, &sbuf, 1) == -1)
			return -1;
	}
	else {
		if (errno != EEXIST)
			return -1;

		arg.buf = &dsbuf;
		if ((*lock = semget(semkey, 0, 0)) == -1)
			return -1;
		for (int i = 0; i < MAX_TRIES; ++i) {
			if (semctl(*lock, 0, IPC_STAT, arg) == -1)
				return -1;
			if (arg.buf->sem_otime != 0)
				break;
			sleep(1);
		}
		if (arg.buf->sem_otime == 0)
			return -1;
	}
	return 0;
}


/**
 * 另一种创建二值信号量的方式，但是没有做出进程间竞态条件的
 * 防范措施，但是在多线程进程中使用还是够用的
 */
int BSem_Create1(lock_t* lock, const char* path, int projid) {
	key_t semkey;

	if (path == NULL || projid < 0 || projid>255)
		*lock = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | IPC_PERM);
	else {
		if ((semkey = ftok(path, projid)) == -1)
			return -1;
		*lock = semget(semkey, 1, IPC_CREAT | IPC_EXCL | IPC_PERM);
	}
	if (*lock == -1)
		return -1;
	return 0;
}


/**
 * 初始化二值信号量的值为指定值
 */
int BSem_Init(lock_t* lock, int initv) {
	union semun arg;
	
	arg.val = 1;
	return semctl(*lock, 0, SETVAL, arg);
}


/** 
 * 删除指定二值信号量
 */
int BSem_Destroy(lock_t* lock) {
	return semctl(*lock, 0, IPC_RMID);
}


/**
 * 试图对信号量值-1，信号调整值+1
 */
int BSem_Sub(lock_t* lock) {
	struct sembuf sbuf;

	sbuf.sem_num = 0;
	sbuf.sem_op = -1;
	sbuf.sem_flg = SEM_UNDO;
	return semop(*lock, &sbuf, 1);
}


/**
 * 试图对信号量+1，信号量-1
 */
int BSem_Add(lock_t* lock) {
	struct sembuf sbuf;

	sbuf.sem_num = 0;
	sbuf.sem_op = 1;
	sbuf.sem_flg = SEM_UNDO;
	return semop(*lock, &sbuf, 1);
}
