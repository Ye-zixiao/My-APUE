#include"MyAPUE.h"

int main(void)
{
	int status;

	if ((status = System("date")) < 0)
		err_sys("System() error\n");
	pr_exit(status);
	putchar('\n');

	if ((status = System("nosuchcmd")) < 0)
		err_sys("System() error\n");
	pr_exit(status);
	putchar('\n');

	if ((status = System("who;exit 44")) < 0)
		err_sys("System() error\n");
	pr_exit(status);
	putchar('\n');

	exit(EXIT_SUCCESS);
}