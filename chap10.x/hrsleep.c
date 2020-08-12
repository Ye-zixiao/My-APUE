#include"../include/MyAPUE.h"

static unsigned int
hrsleep(unsigned int seconds) {
	struct timespec passtime, endtime;

	passtime.tv_sec = seconds;
	passtime.tv_nsec = 0;
	if (nanosleep(&passtime, &endtime) != 0)
		fputs("nanosleeep doesn't pass the time of request\n",
			stderr);
	return endtime.tv_sec;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_sys("usage:hrsleep <seconds>\n");

	pr_now();
	hrsleep(atoi(argv[1]));
	pr_now();
	exit(EXIT_SUCCESS);
}
