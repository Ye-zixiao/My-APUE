#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>


void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

/*mkrmdir*/
int main(int argc, char* argv[])
{
	if (argc < 3) err_sys("usage:mkrmdir [-m|r] dirname\n");
	
	if (strcmp(argv[1], "-m") == 0) {
		struct stat statbuf;
		if (stat("./", &statbuf) < 0)
			err_sys("stat error\n");
		mode_t default_mode = statbuf.st_mode;
		if (mkdir(argv[2], default_mode) < 0)
			err_sys("mkdir error\n");
	}
	else if (strcmp(argv[1], "-r") == 0) {
		if (rmdir(argv[2]) < 0)
			err_sys("rmdir error\n");
	}
	else
		err_sys("usage:mkrmdir [-m|r] dirname\n");

	exit(EXIT_SUCCESS);
}
