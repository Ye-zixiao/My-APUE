#include<stdio.h>
#include<stdlib.h>

static void exit_handler1(void) {
	printf("first exit handler\n");
}

static void exit_handler2(void) {
	printf("second exit handler\n");
}

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(void)
{
	if (atexit(exit_handler2) != 0)
		err_sys("atexit error\n");
	if (atexit(exit_handler1) != 0)
		err_sys("atexit error\n");
	if (atexit(exit_handler1) != 0)
		err_sys("atexit error\n");

	exit(EXIT_SUCCESS);
}