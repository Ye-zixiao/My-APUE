#include "open.h"
#include <string.h>
#include <netinet/in.h>

#define BUFFSIZE (BUFSIZE*2)


int main(int argc, char* argv[])
{
	int nread, fd;
	char buf[BUFFSIZE];
	char line[MAXLINE];

	if (setvbuf(stdout, NULL, _IOLBF, 0) < 0)
		err_sys("setvbuf error");

	printf("prompt> ");
	while (fgets(line, MAXLINE, stdin) != NULL) {
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';
		if ((fd = csopen(line, O_RDONLY)) < 0)
			goto prompt;

		while ((nread = read(fd, buf, BUFFSIZE)) > 0)
			if (write(STDOUT_FILENO, buf, nread) != nread)
				err_sys("write error");
		if (nread < 0)
			err_sys("read error");
		close(fd);
prompt:
		printf("\nprompt> ");
	}
}