/**
 * 使用select实现的sleep_us微秒级休眠函数
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/select.h>

#define SEC2US 1000000L

typedef long int usec_t;

static void sleep_us(usec_t us) {
	struct timeval tval;
	
	tval.tv_usec = us % SEC2US;
	tval.tv_sec = us / SEC2US;
	select(1, NULL, NULL, NULL, &tval);
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
