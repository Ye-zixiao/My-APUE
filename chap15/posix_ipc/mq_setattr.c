#include "../../include/MyAPUE.h"
#include <string.h>
#include <mqueue.h>

int main(int argc, char* argv[])
{
	struct mq_attr attr;
	mqd_t mqd;

	if (argc != 3 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: mq_setattr [+-]n <mq_name>");

	if ((mqd = mq_open(argv[2], O_RDWR)) < 0)
		err_sys("mq_open error");
	if (mq_getattr(mqd, &attr) == -1)
		err_sys("mq_getattr error");
	if (strcmp(argv[1], "-n") == 0)
		attr.mq_flags &= ~O_NONBLOCK;
	else if (strcmp(argv[1], "+n") == 0)
		attr.mq_flags |= O_NONBLOCK;
	else
		err_quit("usage: mq_setattr [+-]n <mq_name>");

	if (mq_setattr(mqd, &attr, NULL) == -1)
		err_sys("mq_setattr error");

	exit(EXIT_SUCCESS);
}
