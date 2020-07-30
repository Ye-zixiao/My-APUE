/*
 * shell上的date命令就是由strftime函数实现的
 * */
#include"MyAPUE.h"
#include<time.h>

int main(void)
{
	time_t nowtime;
	struct tm* ptm;
	char buf[64];

	time(&nowtime);
	if ((ptm = localtime(&nowtime)) == NULL)
		err_sys("localtime error\n");
	if (strftime(buf, 64, "%c %Z\n", ptm) == 0)
		err_sys("strftime error\n");
	fputs(buf,stdout);

	/*if (strptime(buf, "%c %Z", ptm) == NULL)
		err_sys("strptime error\n");
	printf("%d-%d-%d\n", ptm->tm_year + 1900, ptm->tm_mon+1, ptm->tm_mday);*/

	exit(EXIT_SUCCESS);
}
