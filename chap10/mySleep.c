#include"MyAPUE.h"
#include<signal.h>
#include<time.h>
#include<sys/time.h>

#define BufSize 64

static void alarm_handler(int sig){}

unsigned int mySleep(unsigned int seconds) {
	if (signal(SIGALRM, alarm_handler) == SIG_ERR)
		return seconds;
	alarm(seconds);
	pause();
	return alarm(0);
}

void print_now(void) {
	struct timeval timevalbuf;
	struct tm* ptm;
	char buf[BufSize];

	gettimeofday(&timevalbuf, NULL);
	if ((ptm = localtime(&timevalbuf.tv_sec)) == NULL)
		err_sys("localtime error\n");
	if (strftime(buf, BufSize, "%Y-%m-%d %H:%M:%S", ptm) == 0)
		err_sys("strftime error\n");
	printf("now time is %s\n", buf);
}

int main(int argc,char*argv[])
{
	if(argc!=2)
		err_sys("usage:sleep <seconds>\n");

	print_now();
	mySleep(atoi(argv[1]));
	print_now();
	return 0;
}
