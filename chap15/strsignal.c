/**
 * 接受一个信号编码，然后显示其信号意义
 * */
#include "../include/MyAPUE.h"
#include <string.h>

int main(int argc, char* argv[])
{
	int err;

	if (argc != 2)
		err_quit("usage: strsignal <signo>");
	err = atoi(argv[1]);
	printf("signal(%d): %s\n", err, strsignal(err));
	exit(EXIT_SUCCESS);
}
