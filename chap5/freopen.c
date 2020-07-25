/*
 * 打开一个文件与流file进行绑定，然后又用freopen将流与之解绑，
 * 重新打开一个文件与这个流进行绑定，最后输出这个后来绑定的文件
 * 到stdout标准输出中
 * */
#include<stdio.h>
#include<stdlib.h>

#define BUFSIZE 4096

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}


int main(int argc, char* argv[])
{
	if (argc != 3)
		err_sys("usage:reout [false_open_file] [real_open_file]]\n");

	char buf[BUFSIZE];
	FILE* file = fopen(argv[1], "r");
	if (file == NULL)
		err_sys("fopen error\n");
	if (freopen(argv[2], "r", file) == NULL)
		err_sys("freopen error\n");
	if (setvbuf(file, buf, _IOFBF, BUFSIZE) != 0)
		err_sys("setvbuf error\n");

	while (fgets(buf, BUFSIZE, file) != NULL)
		if (fputs(buf, stdout) == EOF)
			err_sys("fputs error\n");
	if (ferror(file))
		err_sys("fgets error\n");

	fclose(file);
	exit(EXIT_SUCCESS);
}
