#include "../include/MyAPUE.h"
#include <string.h>

#define FIFOBUFSIZE 64

int main(int argc, char* argv[])
{
	int fd, nread;
	char buf[FIFOBUFSIZE];

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: openfifo <FIFOName>");
	
	if ((fd = open(argv[1], O_NONBLOCK | O_RDWR)) < 0)
		err_sys("open fifo eror");
	if (write(fd, "hello world\n", 12) != 12)
		err_sys("write error");
	fprintf(stderr, "%s write message(%dbytes) to FIFO\n", argv[0], 12);
	if ((nread = read(fd, buf, FIFOBUFSIZE)) < 0)
		err_sys("read error");

	buf[nread] = 0; close(fd);
	fprintf(stderr, "%s read message(%dbytes) from FIFO: %s",
				argv[0], nread, buf);

	exit(EXIT_SUCCESS);
}
