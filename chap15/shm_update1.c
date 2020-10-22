#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define NLOOPS 1000

struct Shared {
	int value;
	lock_t lock;
};

static inline int update(int* ptr) {
	return (*ptr)++;
}


int main(void)
{
	struct Shared* sptr;
	int shmid;
	pid_t pid;

	if ((shmid = shmget(IPC_PRIVATE, sizeof(struct Shared), IPC_CREAT | IPC_PERM)) == -1)
		err_sys("shmget error");
	if ((sptr = shmat(shmid, 0, 0)) == (void*)-1)
		err_sys("shmat error");
	memset(sptr, 0, sizeof(struct Shared));
	if (BSem_Create1(&sptr->lock, NULL, -1) == -1)
		err_sys("BSem_Create error");
	if (BSem_Init(&sptr->lock, 1) == -1)
		err_sys("BSem_Init error");
	printf("original value: %d\n", sptr->value);

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		//这里我就不设置进程间的谁先谁后
		for (int i = 0; i < NLOOPS; i += 2) {
			if (BSem_Sub(&sptr->lock) == -1)
				err_sys("BSem_Sub error");
			update(&sptr->value);
			if (BSem_Add(&sptr->lock) == -1)
				err_sys("BSem_Add error");
		}
		exit(EXIT_SUCCESS);
	}
	else {
		for (int i = 1; i < NLOOPS; i += 2) {
			if (BSem_Sub(&sptr->lock) == -1)
				err_sys("BSem_Sub error");
			update(&sptr->value);
			if (BSem_Add(&sptr->lock) == -1)
				err_sys("BSem_Add error");
		}
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
	}

	printf("final result: %d\n", sptr->value);
	if (BSem_Destroy(&sptr->lock) == -1)
		err_sys("BSem_Destroy error");
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
		err_sys("shmctl error");
	exit(EXIT_SUCCESS);
}
