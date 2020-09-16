#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/select.h>

int main(int argc, char* argv[])
{
	int ret;
	int fd1, fd2;
	fd_set writefds;

	if (argc!= 2) {
		fprintf(stderr, "usage: %s filename\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	if ((fd1 = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, FILE_MODE)) < 0)
		err_sys("open error");
	fd2 = dup(fd1);
	FD_ZERO(&writefds);
	FD_SET(fd1, &writefds);
	FD_SET(fd2, &writefds);
	if ((ret = select(fd2 + 1, NULL, &writefds, NULL, NULL)) > 0) {
		if (FD_ISSET(fd1, &writefds)) {
			printf("fd1 is useful\n");
			write(fd1, "hello world from fd1\n", 22);
		}
		if (FD_ISSET(fd2, &writefds)) {
			printf("fd2 is useful\n");
			write(fd2, "hello world from fd2\n", 22);
		}
	}
	else {
		printf("ret = %d: %s\n", ret, strerror(errno));
		printf("get here?\n");
	}

	close(fd1);
	close(fd2);
	exit(EXIT_SUCCESS);
}
