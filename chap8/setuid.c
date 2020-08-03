#define _RESUID
#include"MyAPUE.h"

int main(void)
{
	print_resuid();
	if (setuid(getuid()) != 0)
		err_sys("setuid error\n");
	print_resuid();

	exit(EXIT_SUCCESS);
}
