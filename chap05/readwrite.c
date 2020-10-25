#include"apue.h"

#define BUFSIZE 4096

int main(void)
{
	int val;
	char buf[BUFSIZE];

	while ((val = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, val) != val)
			err_sys("output error");
	if (val < 0)
		err_sys("input error");
	exit(EXIT_SUCCESS);
}