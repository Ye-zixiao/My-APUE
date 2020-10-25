#include"apue.h"

static void charactatime(const char* pstr) {
	int ch;
	setbuf(stdout, NULL);
	while ((ch=*pstr++)!='\0')
		putc(ch, stdout);
}

int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0)
		charactatime("output from child process\n");
	else
		charactatime("output from parent process\n");

	exit(0);
}