/*
* 使用stat而不是lstat函数检查指定路径文件的文件类型
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage:mystat [pathname...]\n");

	int i;
	struct stat statbuf;
	for (i = 1; i < argc; ++i) {
		if (stat(argv[i], &statbuf)) {
			fprintf(stderr, "%s stat error\n", argv[1]);
			continue;
		}

		printf("%s: ", argv[i]);
		switch (statbuf.st_mode & __S_IFMT) {
		case __S_IFREG:printf("regular file\n"); break;
		case __S_IFDIR:printf("directory\n"); break;
		case __S_IFBLK:printf("block special\n"); break;
		case __S_IFCHR:printf("character special\n"); break;
		case __S_IFIFO:printf("fifo"); break;
		case __S_IFLNK:printf("soft link\n"); break;
		case __S_IFSOCK:printf("socket\n"); break;
		default:printf("** unknown mode **\n"); break;
		}
	}
	
	exit(EXIT_SUCCESS);
}