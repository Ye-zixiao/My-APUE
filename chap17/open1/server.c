#include "opend.h"

char errmsg[MAXLINE];
char* pathname;
int oflag;


int main(void)
{
	char buf[MAXLINE];
	int nread;

	for (;;) {
		if ((nread = read(STDIN_FILENO, buf, MAXLINE)) < 0)
			err_sys("read error");
		else if (nread == 0)
			break;
		handle_request(buf, nread, STDOUT_FILENO);
	}
	exit(EXIT_SUCCESS);
}