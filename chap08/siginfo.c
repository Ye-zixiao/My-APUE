#include"MyAPUE.h"
#include<sys/wait.h>
#include<signal.h>

void siginfo_status(siginfo_t* infop) {
	//si_signo总是会被设定为SIGCHLD
	if (infop->si_code == CLD_EXITED)
		printf("normal terminal, signal number = %d, exit status = %d\n",
			infop->si_signo, infop->si_status);
	else if (infop->si_code == CLD_KILLED ||
		infop->si_code == CLD_DUMPED) {
		//printf("si_code = %d\n", infop->si_code);
		printf("abnormal terminal, signal number = %d%s\n", infop->si_signo,
			(infop->si_code == CLD_DUMPED) ? ", dump file generated" : "");
	}
	else if (infop->si_code == CLD_STOPPED)
		printf("child stopeped, signal number = %d\n", infop->si_signo);
}

int main(void)
{
	siginfo_t siginfo;
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0)
		exit(7);

	if (waitid(P_PID, pid, &siginfo, WEXITED) != 0)
		err_sys("waitid error\n");
	siginfo_status(&siginfo);

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0)
		abort();

	if (waitid(P_PID, pid, &siginfo, WEXITED) != 0)
		err_sys("watiid error\n");
	siginfo_status(&siginfo);

	if ((pid = fork()) < 0)
		err_sys("fork error\n");
	else if (pid == 0)
		siginfo.si_code /= 0;

	if (waitid(P_PID, pid, &siginfo, WEXITED) != 0)
		err_sys("waitid error\n");
	siginfo_status(&siginfo);

	exit(EXIT_SUCCESS);
}