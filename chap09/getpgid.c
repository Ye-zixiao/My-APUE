#include "../include/MyAPUE.h"
#include <termio.h>

int main(void) {
	printf("pid: %4d, ppid= %4d, pgid= %4d, sid= %4d, session id= %4d\n",
		getpid(), getppid(), getpgid(0),getsid(0),tcgetsid(STDOUT_FILENO));//也可以用getpgrp()函数
	exit(EXIT_SUCCESS);
}
