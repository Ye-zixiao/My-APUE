#include "../../include/MyAPUE.h"
#include <string.h>
#include <mqueue.h>

int main(int argc, char* argv[])
{
	mqd_t mqd;
	struct mq_attr attr;

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: getmqattr <mq_name>");

	if ((mqd = mq_open(argv[1], O_RDONLY)) < 0)
		err_sys("mq_open error");
	if (mq_getattr(mqd, &attr) == -1)
		err_sys("mq_getattr error");
	printf("Maximum # of messages on mqueue :   %ld\n", attr.mq_maxmsg);
	printf("Maximum message size            :   %ld\n", attr.mq_msgsize);
	printf("# of messages currently on queue:   %ld\n", attr.mq_curmsgs);
	
	exit(EXIT_SUCCESS);
}
