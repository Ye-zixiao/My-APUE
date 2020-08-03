#include"MyAPUE.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage:tsys <execute-file>\n");

	int status;
	if ((status = System(argv[1])) < 0)
		err_sys("System() error\n");
	pr_exit(status);

	exit(EXIT_SUCCESS);
}