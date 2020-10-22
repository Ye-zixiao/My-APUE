#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define NLOOPS 1000
#define SIZE sizeof(long)
#define IPC_PERM 0666

static long update(long* ptr) {
	return (*ptr)++;
}


int main(void)
{
	int shmid, counter;
	long* area;
	pid_t pid;

	if ((shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | IPC_EXCL | IPC_PERM)) == -1)
		err_sys("shmget error");
	if ((area = shmat(shmid, 0, 0)) == (void*)-1)
		err_sys("shmat error");
	memset(area, 0, SIZE);
	printf("original value: %ld\n", *area);

	TELL_WAIT();
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		for (int i = 0; i < NLOOPS; i += 2) {
			if ((counter = update(area)) != i)
				err_sys("child: expected %d, but got %ld", i, counter);
			TELL_PARENT(getppid());
			WAIT_PARENT();
		}
		exit(EXIT_SUCCESS);
	}
	else {
		for (int i = 1; i < NLOOPS; i += 2) {
			WAIT_CHILD();
			if ((counter = update(area)) != i)
				err_sys("parent: expected %d, but got %ld", i, counter);
			TELL_CHILD(pid);
		}
	}
	printf("final result: %ld\n", *area);

	if (shmctl(shmid, IPC_RMID, 0) == -1)
		err_sys("shmctl error");
	exit(EXIT_SUCCESS);
}
