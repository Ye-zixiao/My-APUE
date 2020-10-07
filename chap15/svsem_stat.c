/**
 * 获取指定System V信号量集的状态
 */
#include "../include/MyAPUE.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

union semun {
	int val;
	struct semid_ds* buf;
	unsigned short* array;
};

int main(int argc, char* argv[])
{
	struct semid_ds dsbuf;
	union semun arg, dummy;
	int semid;

	if (argc != 2 || strncmp("--help", argv[1], 6) == 0)
		err_quit("usage: %s <semid>", argv[0]);

	semid = atoi(argv[1]);
	arg.buf = &dsbuf;
	//获取指定信号量集的状态
	if (semctl(semid, 0, IPC_STAT, arg) == -1)
		err_sys("semctl error");
	printf("Semaphore changed: %s\n", transformTime(dsbuf.sem_ctime, NULL));
	printf("Semaphore last op: %s\n", transformTime(dsbuf.sem_otime, NULL));

	//获取每一个信号量上的值
	arg.array = calloc(dsbuf.sem_nsems, sizeof(unsigned short));
	if (arg.array == NULL)
		err_sys("calloc error");
	if (semctl(semid, 0, SETALL, arg) == -1)
		err_sys("semctl error");
	printf("Sem #  Value  SEMPID  SEMCNT  SEMZCNT\n");
	for (int i = 0; i < dsbuf.sem_nsems; ++i)
		printf("%3d   %5d   %5d   %5d    %5d\n", i, arg.array[i],
			semctl(semid, i, GETPID, dummy),
			semctl(semid, i, GETNCNT, dummy),
			semctl(semid, i, GETZCNT, dummy));
	
	exit(EXIT_SUCCESS);
}
