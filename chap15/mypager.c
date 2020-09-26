#include "../include/MyAPUE.h"
#include <string.h>

#define DEFAULT_PAGER "${PAGER:-more}"

int main(int argc, char* argv[])
{
	char DataBuf[BUFSIZE];
	FILE* fpin, * fpout;

	if (argc != 2)
		err_quit("usage: %s <filename>", argv[0]);
	if ((fpin = fopen(argv[1], "r")) == NULL)
		err_sys("fopen error");
	if ((fpout = Popen(DEFAULT_PAGER, "w")) == NULL)
		err_sys("Popen error");

	while (fgets(DataBuf, BUFSIZE, fpin) != NULL) {
		if (fputs(DataBuf, fpout) == EOF)
			err_sys("fputs error");
	}
	if (ferror(fpin))
		err_sys("fgets error");
	if (Pclose(fpout) < 0)
		err_sys("Pclose error");
	if (fclose(fpin) < 0)
		err_sys("fclose error");

	exit(EXIT_SUCCESS);
}
