#include "../../include/MyAPUE.h"
#include <string.h>
#include <mqueue.h>

#undef BUFSIZE
#define BUFSIZE 4096*2
#define SENDMSG "hello world from server"

int main(int argc, char* argv[])
{
	int val;
	mqd_t mqd;
	char databuf[BUFSIZE];

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: pmqServer <mq_name>");

	if ((mqd = mq_open(argv[1], O_CREAT | O_EXCL | O_RDWR, FILE_MODE, NULL)) < 0) {
		if (errno == EEXIST) {
			if ((mqd = mq_open(argv[1], O_RDWR)) == -1)
				err_sys("mq_open error");
		}
		else
			err_sys("mq_open error");
	}

	while ((val = mq_receive(mqd, databuf, BUFSIZE, 0)) != -1) {
		if (strncmp(databuf, "server end", 10) == 0) {
			mq_send(mqd, databuf, strlen(databuf) + 1, 0);
			printf("server end\n");
			mq_close(mqd);
			mq_unlink(argv[1]);
			break;
		}
		printf("client: %s", databuf);
		if (mq_send(mqd, SENDMSG, strlen(SENDMSG) + 1, 0) == -1)
			err_sys("mq_send error");
	}

	exit(EXIT_SUCCESS);
}
