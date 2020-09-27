#include "../include/MyAPUE.h"
#include <string.h>

#define MAXLINE 1024

int main(void)
{
	char line[MAXLINE];
	int int1, int2;

	//将协同进程的stdin、stdout的缓冲类型都设置为行缓冲方式
	if (setvbuf(stdin, NULL, _IOLBF, 0) != 0)
		err_sys("setvbuf error");
	if (setvbuf(stdout, NULL, _IOLBF, 0) != 0)
		err_sys("setvbuf error");

	while (fgets(line, MAXLINE, stdin) != NULL) {
		if (sscanf(line, "%d%d", &int1, &int2) == 2) {
			if (printf("%d\n", int1 + int2) == EOF)
				err_sys("printf error");
		}
		else {
			if (printf("invalid args\n") == EOF)
				err_sys("printf error");
		}
		//fflush(stdout);
	}

	exit(EXIT_SUCCESS);
}
