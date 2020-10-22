#include "../include/MyAPUE.h"
#include <sys/mman.h>
#include <sys/shm.h>

#define NLOOPS 1000
#define SIZE sizeof(long)
#define SHM_PERM 0666

static int update(long* ptr) {
	return (*ptr)++;
}

int main(void)
{
	int shmid, counter;
	pid_t pid;
	void* area;

	if ((shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | IPC_EXCL | SHM_PERM)) == -1)
		err_sys("shmget error");
	if ((area = shmat(shmid, 0, 0)) == (void*)-1)
		err_sys("shmat error");
	*(long*)area = 0L;
	printf("original value: %ld\n", *(long*)area);

	/* 对存储映射区中的数据交替由父子进程递增 */
	TELL_WAIT();
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		for (int i = 0; i < NLOOPS; i += 2) {
			if ((counter = update((long*)area)) != i)
				err_quit("child: expected %d, got %d", i, counter);
			
			TELL_PARENT(getppid());
			WAIT_PARENT();
		}
	}
	else {
		for (int i = 1; i < NLOOPS; i += 2) {
			WAIT_CHILD();

			if ((counter = update((long*)area)) != i)
				err_quit("parent: expected %d, got %d", i, counter);

			TELL_CHILD(pid);
		}
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
		printf("final value: %ld\n", *(long*)area);
		if (shmctl(shmid, IPC_RMID, NULL) == -1)
			err_sys("shmctl error");
	}

	exit(EXIT_SUCCESS);
}
