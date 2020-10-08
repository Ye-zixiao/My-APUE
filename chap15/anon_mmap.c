#include "../include/MyAPUE.h"
#include <sys/mman.h>
#include <sys/shm.h>

#define NLOOPS 1000
#define SIZE sizeof(long)

static int update(long* ptr) {
	return (*ptr)++;
}

int main(void)
{
	int counter;
	pid_t pid;
	void* area;

	if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED,
		-1, 0)) == MAP_FAILED)
		err_sys("mmap error");
	*(long*)area = 0;

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
	}

	exit(EXIT_SUCCESS);
}
