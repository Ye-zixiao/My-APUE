#include "../include/MyAPUE.h"
#include <string.h>

#define PAGER "${PAGER:-more}"

int main(int argc, char* argv[])
{
	char databuf[BUFSIZE];
	FILE* fpin, *fpout;

	if (argc != 2)
		err_quit("usage: %s <filename>", argv[0]);

	if ((fpin = fopen(argv[1], "r")) == NULL)
		err_sys("fopen error");
	//创建管道并fork shell程序执行cmdstring命令
	if ((fpout = popen(PAGER, "w")) == NULL)
		err_sys("popen error");

	//将数据写入到popen返回的输出文件指针中
	while (fgets(databuf, BUFSIZE, fpin) != NULL) {
		if (fputs(databuf, fpout) == EOF)
			err_sys("fputs error");
	}
	if (ferror(fpin))
		err_sys("fgets error");
	if (pclose(fpout) < 0)
		err_sys("pclose error");
	if (fclose(fpin) < 0)
		err_sys("fclose error");

	exit(EXIT_SUCCESS);
}
