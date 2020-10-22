#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define NLOOPS 1000
#define SIZE sizeof(int)
#define LOCKFILEPATH "tempfile"

static inline int update(int* ptr) {
	return (*ptr)++;
}


int main(void)
{
	int shmid, fd, * sptr;
	pid_t pid;

	if ((fd = open(LOCKFILEPATH, O_CREAT | O_WRONLY, FILE_MODE)) < 0)
		err_sys("open error");
	if ((shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | IPC_PERM)) == -1)
		err_sys("shmget error");
	if ((sptr = shmat(shmid, 0, 0)) == (void*)-1)
		err_sys("shmat error");
	memset(sptr, 0, SIZE);
	printf("original value: %d\n", *sptr);

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		for (int i = 0; i < NLOOPS; i += 2) {
			//if (lockfile(fd) == -1)
			//	err_sys("lockfile error");
			if (writew_lock(fd, 0, SEEK_SET, 0) == -1)
				err_sys("writew_lock error");
			update(sptr);
			if (un_lock(fd, 0, SEEK_SET, 0) == -1)
				err_sys("un_lock error");
		}
		exit(EXIT_SUCCESS);
	}
	else {
		for (int i = 1; i < NLOOPS; i += 2) {
			//if (lockfile(fd) == -1)
				//err_sys("lockfile error");
			if (writew_lock(fd, 0, SEEK_SET, 0) == -1)
				err_sys("writew_lock error");
			update(sptr);
			if (un_lock(fd, 0, SEEK_SET, 0) == -1)
				err_sys("un_lock error");
		}
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
	}

	printf("final result: %d\n", *sptr);
	if (close(fd) == -1)
		err_sys("close error");
	if (unlink(LOCKFILEPATH) == -1)
		err_sys("unlink error");
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
		err_sys("shmctl error");
	exit(EXIT_SUCCESS);
}
