/**
 * 使用ppoll实现的sleep_us微秒级休眠函数
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/poll.h>

#define SEC2NSEC 1000000000L
#define USEC2NSEC 1000L
#define SEC2USEC 1000000L

typedef long usec_t;


static void sleep_us(usec_t us) {
	struct timespec timesp;

	timesp.tv_nsec = us % SEC2USEC * USEC2NSEC;
	timesp.tv_sec = us / SEC2USEC;
	ppoll(NULL, 0, &timesp, NULL);
}


int main(int argc, char* argv[])
{
	usec_t us;

	if (argc != 2 || strncmp(argv[1], "--help", 6) == 0)
		err_quit("usage: %s <useconds>", argv[0]);

	us = (usec_t)atol(argv[1]);
	printf("time: %s\n", currTime(NULL));
	sleep_us(us);
	printf("time: %s\n", currTime(NULL));
	exit(EXIT_SUCCESS);
}
