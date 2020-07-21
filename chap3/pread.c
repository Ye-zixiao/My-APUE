#include"apue.h"
#include<fcntl.h>

#define BUFSIZE 4096

int main(void)
{
	int fd, n;
	char buf[BUFSIZE];
	if((fd=open("outfile",O_RDONLY)) < 0)
		err_sys("open error");

	if ((n = pread(fd, buf, BUFSIZE, 0)) > 0) {
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");
	}
	else if(n<0)
		err_sys("pread error");

	while ((n = read(fd, buf, BUFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");
	if (n < 0)
		err_sys("read error");

	exit(0);
}
