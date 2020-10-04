#include "MyAPUE.h"
#include <sys/ipc.h>
#include <sys/sem.h>

#define DEFAULT_PATH "/tmp"
#define DEFAULT_PROID 24
#define DEFAULT_PERM 0666
#define DEFAULT_CNT 100000

union semun {
	int val;
	struct semid_ds* buf;
	unsigned short* array;
};

typedef int lock_t;

struct SharedVal {
	FILE* fp;
	lock_t lock;
} svalue;


static int 
lock_init(lock_t* lock) {
	key_t semkey;
	union semun init_val;
	
	init_val.val = 1;
	if ((semkey = ftok(DEFAULT_PATH, DEFAULT_PROID)) == -1)
		return -1;
	while ((*lock = semget(semkey, 1, IPC_CREAT | IPC_EXCL | DEFAULT_PERM)) == -1) {
		if (errno == EEXIST) {
			if ((*lock = semget(semkey, 0, 0)) == -1)
				return -1;
			if (semctl(*lock, 0, IPC_RMID) == -1)
				return -1;
			fprintf(stderr, "old System V semaphore(id=%d) removed\n",
				*lock);
		}
		else {
			return -1;
		}
	}
	if (semctl(*lock, 0, SETVAL, init_val) == -1) {
		semctl(*lock, 0, IPC_RMID);
		return -1;
	}
	return 0;
}

static int 
lock_destroy(lock_t* lock) {
	return semctl(*lock, 0, IPC_RMID) == -1 ? -1 : 0;
}

static int 
lock(lock_t* lock) {
	struct sembuf sbuf;

	sbuf.sem_num = 0;
	sbuf.sem_op = -1;
	sbuf.sem_flg = 0;
	if (semop(*lock, &sbuf, 1) == -1)
		return -1;
	return 0;
}

static int 
unlock(lock_t* lock) {
	struct sembuf sbuf;

	sbuf.sem_num = 0;
	sbuf.sem_op = 1;
	sbuf.sem_flg = 0;
	if (semop(*lock, &sbuf, 1) == -1)
		return -1;
	return 0;
}

int main(void)
{
	int temp;
	pid_t pid;

	if ((svalue.fp = fopen("/tmp/testfile", "w+")) == NULL)
		err_sys("fopen error");
	fprintf(svalue.fp, "%d", 0);
	fflush(svalue.fp);
	if (lock_init(&svalue.lock) == -1)
		err_sys("lock_init error");

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		for (int i = 0; i < DEFAULT_CNT; i++) {
			if (lock(&svalue.lock) == -1)
				err_sys("lock error");
			rewind(svalue.fp);
			fscanf(svalue.fp, "%d", &temp);
			rewind(svalue.fp);
			fprintf(svalue.fp, "%d", ++temp);
			fflush(svalue.fp);
			if (unlock(&svalue.lock) == -1)
				err_sys("unlock error");
		}
		_exit(EXIT_SUCCESS);
	}
	else {
		for (int i = 0; i < DEFAULT_CNT; ++i) {
			if (lock(&svalue.lock) == -1)
				err_sys("lock error");
			rewind(svalue.fp);
			fscanf(svalue.fp, "%d", &temp);
			rewind(svalue.fp);
			fprintf(svalue.fp, "%d", ++temp);
			fflush(svalue.fp);
			if (unlock(&svalue.lock) == -1)
				err_sys("unlock error");
		}
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
		rewind(svalue.fp);
		fscanf(svalue.fp, "%d", &temp);
		printf("svalue.fp's shared value: %d\n", temp);
		lock_destroy(&svalue.lock);
	}

	exit(EXIT_SUCCESS);
}
