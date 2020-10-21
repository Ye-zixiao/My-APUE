#include "../include/MyAPUE.h"
#include <string.h>

int main(int argc, char* argv[])
{
	char buf[BUFSIZE];
	size_t len;
	FILE* fp;

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: popen_tostderr <cmdstring>");

	if ((fp = Popen(argv[1], "r")) == NULL)
		err_sys("Popen error");
	while (fgets(buf, BUFSIZE, fp) != NULL) {
		debug();
		len = strlen(buf);
		if (write(STDOUT_FILENO, buf, len) != len)
			err_sys("write error");
	}
	if (ferror(fp))
		err_sys("fgets error");
	Pclose(fp);

	exit(EXIT_SUCCESS);
}
