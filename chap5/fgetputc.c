#include"apue.h"

int main(void)
{
	int ch;
	while ((ch = fgetc(stdin)) != EOF)
		if (fputc(ch, stdout) == EOF)
			err_sys("output error");
	if (ferror(stdin))
		err_sys("input error");
	exit(EXIT_SUCCESS);
}