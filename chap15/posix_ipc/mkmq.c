#include "../../include/MyAPUE.h"
#include <string.h>
#include <mqueue.h>

#define NAMESIZE 256

void check_n_init(char* argv[], int argc, int* flags, char* name) {
	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: mkmq [-x] <mq_name>");
	else if (argc == 2)
		strcpy(name, argv[1]);
	else if (argc == 3) {
		if (strcmp(argv[1], "-x") != 0)
			err_quit("usage: mkmq [-x] <mq_name>");
		*flags |= O_EXCL;
		strcpy(name, argv[2]);
	}
}

int main(int argc, char* argv[])
{
	char name[NAMESIZE];
	int flags = O_CREAT;

	check_n_init(argv, argc, &flags, name);
	if (mq_open(name, flags, FILE_MODE, NULL) < 0)
		err_sys("mq_open error");
	exit(EXIT_SUCCESS);
}
