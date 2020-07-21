#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>

#define BUFSIZE 4096

/*
 * stdin(利用重定向进行指定输入文件流)-->fd
 *
 * 虽然是同步写入，但是写入的效率很低，类似于在磁盘上做4K写入
 */


void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(-1);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage:command outfile_name\n");
		exit(-1);
	}
	int val, fd;
	char buf[BUFSIZE];
	if ((fd = open(argv[1], O_WRONLY | O_CREAT | O_SYNC | O_TRUNC)) < 0)
		err_sys("open error\n");

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("get file status flags error\n");
	if (val & O_SYNC)printf("synchronous write valid\n");
	else printf("synchronous write invalid\n");

	while ((val = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
		if (write(fd, buf, val) != val)
			err_sys("write error\n");
	if (val < 0)
		err_sys("read error\n");
	exit(0);
}
