#include "../../include/MyAPUE.h"
#include <string.h>
#include <mqueue.h>

#undef BUFSIZE
#define BUFSIZE 4096*2

int main(int argc, char* argv[])
{
	mqd_t mqd;
	char databuf[BUFSIZE];

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: pmqServer <mq_name>");

	if ((mqd = mq_open(argv[1], O_RDWR)) == -1)
		err_sys("mq_open error");

	while (fgets(databuf, BUFSIZE, stdin) != NULL) {
		if (mq_send(mqd, databuf, strlen(databuf) + 1, 0) == -1)
			err_sys("mq_send error");
		if (mq_receive(mqd, databuf, BUFSIZE, 0) == -1)
			err_sys("mq_receive error");
		if (strncmp(databuf, "server end", 10) == 0) {
			printf("client end\n");
			mq_close(mqd);
			break;
		}
		printf("server: %s\n", databuf);
		fflush(stdout);
	}
	if (ferror(stdin))
		err_sys("ferror error");

	exit(EXIT_SUCCESS);
}

