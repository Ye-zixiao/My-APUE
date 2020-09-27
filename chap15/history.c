#include "../include/MyAPUE.h"

#define MAXLINE 1024

int main(void)
{
	char buf[MAXLINE];
	FILE* outfile;

	if (setvbuf(stdin, buf, _IOLBF, MAXLINE) != 0)
		err_sys("setvbuf error");
	if ((outfile = fopen("/tmp/history", "a")) == NULL)
		err_sys("fopen outfile error");

	while (fgets(buf, MAXLINE, stdin) !=NULL) {
		fputs(buf, outfile);
		fputs(buf, stdout);
		fflush(NULL);
	}

	if (fclose(outfile) == EOF)
		err_sys("fclose error");
	exit(EXIT_SUCCESS);
}
