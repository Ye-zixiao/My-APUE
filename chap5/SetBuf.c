#include<stdio.h>
#include<stdlib.h>
#include"Buf.h"

void SetBuf(FILE* restrict fp, char* restrict buf) {
	if (setvbuf(fp, buf, buf ? _IOFBF : _IONBF, BUFSIZ) != 0)
		err_sys("setvbuf error\n");
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_sys("usage:out <filename>\n");
	
	char buf[BUFSIZ];
	FILE* fp = fopen(argv[1], "r");
	if (fp == NULL)
		err_sys("fopen error\n");

	/*在实际中setvbuf当mode=_IOFBF时，并不能表现出它该有的缓冲类型，
		而被设置为行缓冲，暂时不知道为什么出现这种情况*/
	SetBuf(fp, buf);
	//SetBuf(fp, NULL);

	if (getc(fp) == EOF)
		err_sys("getc error\n");
	print_iostat("file stream", fp);

	fclose(fp);
	exit(EXIT_SUCCESS);
}