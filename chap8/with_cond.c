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

	TELL_WAIT();
	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		//WAIT_PARENT();
		charactatime("output from child process\n");
		TELL_PARENT(getppid());
	}
	else{
		WAIT_CHILD();
		charactatime("output from parent process\n");
		//TELL_CHILD(pid);//父进程先行
	}

	exit(0);
}