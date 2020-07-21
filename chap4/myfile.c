#include"apue.h"
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>


int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("Usage:myfile filename");

	int i;
	struct stat buf;
	char* pchar;

	for (i = 1; i < argc; ++i) {
		printf("%s:", argv[i]);
		if (lstat(argv[i], &buf)<0) {
			err_ret("lstat error");
			continue;
		}
		if (S_ISREG(buf.st_mode))
			pchar = "regular";
		else if (S_ISDIR(buf.st_mode))
			pchar = "directory";
		else if (S_ISCHR(buf.st_mode))
			pchar = "character special";
		else if (S_ISBLK(buf.st_mode))
			pchar = "block special";
		else if (S_ISFIFO(buf.st_mode))
			pchar = "fifo";
		else if (S_ISLNK(buf.st_mode))
			pchar = "symbolic link";
		else if (S_ISSOCK(buf.st_mode))
			pchar = "socket";
		else pchar = "** unknown mode **";
		printf("%s\n",pchar);
	}
	exit(0);
}