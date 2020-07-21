/*
 * 这个程序用以显示指定文件在磁盘上分配了多少个block（512B字节）
 * */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage:command filename...\n");

	int i;
	struct stat statbuf;

	for (i = 1; i < argc; ++i) {
		if (stat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "stat read error\n");
			++i; continue;
		}
		printf("%s filesize:%ld blocksize:%ld\n",
			argv[i], statbuf.st_size, statbuf.st_blocks);
	}

	exit(0);
}
