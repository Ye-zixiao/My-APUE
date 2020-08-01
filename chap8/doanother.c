#include"MyAPUE.h"
#include<sys/wait.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage:doanother <execute-file> <args...>\n");

	pid_t pid;
	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		if (execvp(argv[1], &argv[1]) == -1)
			err_sys("execvp error\n");
	}
	else {
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("watipid error\n");
	}
	
	exit(EXIT_SUCCESS);
}