#include"MyAPUE.h"
#include<string.h>
#include<sys/time.h>
#include<time.h>

const char* getweek(int w) {
	switch (w) {
	case 1:return "Monday";
	case 2:return "Tuesday";
	case 3:return "Wednesday";
	case 4:return "Thursday";
	case 5:return "Friday";
	case 6:return "Saturday";
	case 0:return "Sunday";
	default:return NULL;
	}
}

int main(void)
{
	struct timeval timevalbuf;
	struct tm* ptm;
	gettimeofday(&timevalbuf, NULL);
	if((ptm=localtime(&(timevalbuf.tv_sec)))==NULL)
		err_sys("localtime error\n");

	printf("%d-%d-%d %d:%d:%d %s\n", ptm->tm_year+1900, ptm->tm_mon+1,
		ptm->tm_mday, ptm->tm_hour, ptm->tm_min,ptm->tm_sec,getweek(ptm->tm_wday));

	exit(EXIT_SUCCESS);
}