#include "../include/MyAPUE.h"

int main(void)
{
	char DataBuf[BUFSIZE];
	FILE* fpin;

	//if ((fpin = Popen("./history", "r")) == NULL)
	if ((fpin = Popen("./myuclc", "r")) == NULL)
		err_sys("Popen error");
	while (1) {
		fputs("prompt> ", stdout);
		fflush(stdout);
		if (fgets(DataBuf, BUFSIZE, fpin) == NULL)
			break;
		if (fputs(DataBuf, stdout) == EOF)
			err_sys("fputs error to pipe");
	}
	if (Pclose(fpin) < 0)
		err_sys("Pclose error");
	putchar('\n');

	exit(EXIT_SUCCESS);
}
