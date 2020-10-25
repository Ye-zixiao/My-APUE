//#include"MyAPUE.h"
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<errno.h>

int System(const char* cmdstring);

int main(void){
	return system("date +%Y-%m-%d");
}

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
