#include"../include/MyAPUE.h"

/*
 * 这里并没有使用书中的TELL-WAIT组合，而是采用简化的改编版，
 * 采用rewind进行回溯重写重读
 *
 * */

static sigset_t blocksigs, oldsigs, zerosigs;
static volatile sig_atomic_t sigflag;

static void sig_usr(int signo) {
	sigflag = 1;
}

void tw_init(void) {
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("signal error\n");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("signal error\n");
	sigemptyset(&blocksigs);
	sigemptyset(&zerosigs);
	sigaddset(&blocksigs, SIGUSR1);
	sigaddset(&blocksigs, SIGUSR2);
	if (sigprocmask(SIG_BLOCK, &blocksigs, &oldsigs) != 0)
		err_sys("sigprocmask error\n");
}

void Tell(pid_t pid, int signo) {
	kill(pid, signo);
}

void Wait(void) {
	while (sigflag == 0)
		sigsuspend(&zerosigs);
	sigflag = 0;
}

void tw_quit(void) {
	if (sigprocmask(SIG_SETMASK, &oldsigs, NULL) != 0)
		err_sys("sigprocmask error\n");
}


int main(int argc,char*argv[])
{
	FILE* fp;
	pid_t pid;
	int count = 100;
	int private_value;

	if (argc == 2)
		count = atoi(argv[1]);

	tw_init();
	if ((fp = fopen("iofile", "w+")) == NULL)
		err_sys("fopen error\n");
	fputs("0", fp);
	fflush(fp);

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0) {
		for (int i = 0; i < count; ++i) {		//读写的定位为同一个
			rewind(fp);
			fscanf(fp, "%d", &private_value);
			rewind(fp);
			fprintf(fp, "%d\n", ++private_value);
			fflush(fp);	
			//一定要保证使用fflush进行强制刷新，否则父进程可能不会读到最新值

			printf("child  increase value to %d\n", private_value);
			Tell(getppid(), SIGUSR1);
			Wait();
		}
	}
	else {
		for (int i = 0; i < count; ++i) {
			Wait();
			rewind(fp);
			fscanf(fp, "%d", &private_value);
			rewind(fp);
			fprintf(fp, "%d\n", ++private_value);
			fflush(fp);

			printf("parent increase value to %d\n", private_value);
			Tell(pid, SIGUSR2);
		}
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error\n");
	}

	fclose(fp);
	exit(EXIT_SUCCESS);
}
