#include "MyAPUE.h"
#include <time.h>
#include <sys/time.h>

static void isChildExisted(pid_t pid) {
	struct timeval timebuf;
	struct tm* ptm;
	char buf[64];

	gettimeofday(&timebuf, NULL);
	if ((ptm = localtime(&timebuf.tv_sec)) == NULL)
		err_sys("localtime error\n");
	if (strftime(buf, 64, "%c", ptm) == 0)
		err_sys("strftime error\n");
	printf("Now time is %s\n", buf);

	if (kill(pid, 0) == -1) {
		if (errno == ESRCH)
			printf("child(pid: %d) is not exited\n",
				pid);
	}
	else {
		printf("child(pid: %d) is exited\n", pid);
	}
}

int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		sleep(2);
	}
	else {
		isChildExisted(pid);
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error\n");
		isChildExisted(pid);
	}

	exit(EXIT_SUCCESS);
}
