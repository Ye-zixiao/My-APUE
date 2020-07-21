#include"apue.h"
#include<fcntl.h>
//这个函数在读写权限上还是有问题的

int main(int argc, char** argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage:mytouch filename\n");
		exit(0);
	}
	int fd;
	if ((fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL | O_TRUNC)) < 0)
		err_sys("this file has existed");
	else
		close(fd);
	exit(0);
}
