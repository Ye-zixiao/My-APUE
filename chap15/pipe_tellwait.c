#include "../include/MyAPUE.h"

static int pfds1[2], pfds2[2];

void TELL_WAIT1(void) {
	if (pipe(pfds1) < 0 || pipe(pfds2) < 0)
		err_sys("pipe error");
}


void TELL_PARENT1(pid_t pid) {
	if (write(pfds1[1], "c", 1) != 1)
		err_sys("write error");
}

void TELL_CHILD1(pid_t pid) {
	if (write(pfds2[1], "p", 1) != 1)
		err_sys("write error");
}

void WAIT_PARENT1(void) {
	char c;

	if (read(pfds2[0], &c, 1) != 1)
		err_sys("read error");
	if (c != 'p')
		err_quit("WAIT_PARENT1: incorrent data");
}

void WAIT_CHILD1(void) {
	char c;

	if (read(pfds1[0], &c, 1) != 1)
		err_sys("read error");
	if (c != 'c')
		err_quit("WAIT_CHILD1: incorrent data");
}

int main(void)
{
	pid_t pid;

	TELL_WAIT1();
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		printf("hello world from child\n");
		TELL_PARENT1(getppid());
	}
	else {
		WAIT_CHILD1();
		printf("hello world from parent\n");
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
	}

	exit(EXIT_SUCCESS);

}
