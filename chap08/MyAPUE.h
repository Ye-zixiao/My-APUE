#ifndef _MY_AUPE_H_
#define _MY_APUE_H_

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#ifndef _GNU_SOURCE
#define	_GNU_SOURCE
#endif

#include<stdio.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/wait.h>

#define BUFSIZE 4096


/*	-----------------------
*	some useful function
*	-----------------------
*/

void err_sys(const char* msg) {
	fputs(msg,stderr);
	exit(EXIT_FAILURE);
}

void pr_exit(int status) {
	if (WIFEXITED(status))//正常终止
		printf("normal termination,   exit status   = %d\n",
			WEXITSTATUS(status));
	else if (WIFSIGNALED(status))//异常终止
		printf("abnormal termination, signal number = %d%s\n",
			WTERMSIG(status),
#ifdef WCOREDUMP
			WCOREDUMP(status) ? " core file generated)" : "");
#else
		"");
#endif
	else if (WIFSTOPPED(status))//进程暂停
		printf("child stopped,        signal number = %d\n",
			WSTOPSIG(status));
}

#ifdef _RESUID
void print_resuid(void) {
	uid_t ruid, euid, suid;
	if (getresuid(&ruid, &euid, &suid) == -1)
		err_sys("getresuid error\n");
	printf("ruid: %d, euid: %d, suid: %d\n",
		ruid, euid, suid);
}
#endif

int System(const char* cmdstring) {
	if (cmdstring == NULL)
		return 1;

	pid_t pid;
	int status;
	if ((pid = fork()) < 0)
		status = -1;
	else if (pid == 0) {
		/*
			System--->fork-exec出shell------>shell fork-exec出cmdstring（读起来怪怪的😂）
		*/
		execl("/usr/bin/sh", "sh", "-c", cmdstring, (char*)NULL);
		_exit(127);
	}
	else {
		while (waitpid(pid, &status, 0) < 0) {
			if (errno == EINTR) {
				status = -1;
				break;
			}
		}
	}

	return status;
}
/*
1、fork、waitpid失败，system返回-1
2、execl失败返回127
3、正常返回那么就是shell的退出状态
*/


#endif
