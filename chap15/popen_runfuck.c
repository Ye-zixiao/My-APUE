/**
 * 让popen()函数去执行一个不存在的命令
 */
#include "../include/MyAPUE.h"

int main(void)
{
	FILE* fp;
	int err;

	if ((fp = Popen("fuck", "r")) == NULL)
		err_sys("popen error");
	err = Pclose(fp);
	fprintf(stderr, "pclose return cmdstring's status: %d\n", err);
	exit(EXIT_SUCCESS);
}
