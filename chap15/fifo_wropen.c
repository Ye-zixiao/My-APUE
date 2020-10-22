#include "../include/MyAPUE.h"

#define FIFOFILE "temp.fifo"

int main(void)
{
	int fd;

	if (mkfifo(FIFOFILE, FILE_MODE) == -1)
		err_sys("mkfifo error");
	if ((fd = open(FIFOFILE, O_RDONLY | O_NONBLOCK)) < 0)
		err_sys("open error");
	if ((fd = open(FIFOFILE, O_WRONLY)) < 0)
		err_sys("open error");
	if (clr_fd(fd, O_NONBLOCK) == -1)
		err_sys("clr_fd error");

	exit(EXIT_SUCCESS);
}
