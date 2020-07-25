#include<stdio.h>
#include<stdlib.h>

#define BUFSIZE 4096

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	if (argc != 2)
		err_sys("usage:out <filename>\n");

	char outbuf[BUFSIZE];
	char inbuf[BUFSIZE];
	char buf[BUFSIZE];
	FILE* file = fopen(argv[1], "r");
	if (file == NULL)
		err_sys("fopen error\n");
	//实际上也是这样的，每一个流都管理着一个与之相关的buffer，除非这个流采用无缓冲机制
	if (setvbuf(file, inbuf, _IOFBF, BUFSIZE) != 0)
		err_sys("file setvbuf error\n");
	if (setvbuf(stdout, outbuf, _IOFBF, BUFSIZE) != 0)
		err_sys("stdout setvbuf error\n");


	while (fgets(buf, BUFSIZE, file) != NULL)
		if (fputs(buf, stdout) == EOF)
			err_sys("fputs error\n");
	if (ferror(file))
		err_sys("fgets error\n");

	if (fclose(file))
		err_sys("fclose error\n");
	exit(EXIT_SUCCESS);
}
