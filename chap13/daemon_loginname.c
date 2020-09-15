#include "../include/MyAPUE.h"
#include <syslog.h>
#include <string.h>
#include <fcntl.h>

#define LOGFILE "/tmp/logfile"

int main(int argc, char* argv[])
{
	char* username;
	char* cmd;
	FILE* fp;

	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	daemonize(cmd);
	username = getlogin();
	if ((fp = fopen(LOGFILE, "w")) != NULL) {
		if (username == NULL)
			fprintf(fp, "no login name\n");
		else
			fprintf(fp, "login name: %s\n", username);
		fclose(fp);
	}

	exit(EXIT_SUCCESS);
}

/*
	在Linux上用户层守护进程仍然可以打印出loginname，这个
	与APUE书本上的答案不同，可能跟Linux的版本不同有关
*/
