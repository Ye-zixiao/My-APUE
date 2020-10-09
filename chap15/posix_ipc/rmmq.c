#include "../../include/MyAPUE.h"
#include <string.h>
#include <mqueue.h>

int main(int argc, char* argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: rmmq <mq_name>");
	if (mq_unlink(argv[1]) == -1)
		err_sys("mq_unlink error");
	exit(EXIT_SUCCESS);
}
