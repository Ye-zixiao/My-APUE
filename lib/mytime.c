#include "MyAPUE.h"

#define TIMESTRLEN 64


/**
 * 非线程安全地获取当前的时间
 */
const char* currTime(const char* format) {
	static char buf[TIMESTRLEN];
	struct timeval timevalbuf;
	struct tm* ptm;

	gettimeofday(&timevalbuf, NULL);
	if ((ptm = localtime(&timevalbuf.tv_sec)) == NULL)
		return NULL;
	if (format == NULL)
		strftime(buf, TIMESTRLEN, "%H:%M:%S", ptm);
	else
		strftime(buf, TIMESTRLEN, format, ptm);
	return buf;
}


/**
 * 非线程安全地将给定time_t值变换为格式化时间
 */
const char* transformTime(time_t t, const char* format) {
	static char buf[TIMESTRLEN];
	struct tm* ptm;

	if ((ptm = localtime(&t)) == NULL)
		return NULL;
	if (format == NULL)
		strftime(buf, TIMESTRLEN, "%c", ptm);
	else
		strftime(buf, TIMESTRLEN, format, ptm);
	return buf;
}



/*
	用来帮助pthread_cond_timewait这样的函数生成绝对的定时时间
*/
void get_abstime(struct timespec* tsp, long seconds) {
	if (clock_gettime(CLOCK_REALTIME, tsp) != 0)
		err_sys("clock_gettime error\n");
	tsp->tv_sec += seconds;
}

