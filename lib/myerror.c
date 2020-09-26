#include "../include/MyAPUE.h"
#include <stdarg.h>
#include <string.h>

#define MAX 200

static
void err_doit(int iserror, int error, const char* fmt, va_list ap) {
	char buf[MAX];

	//暂时先换存在一个字符数组中
	vsnprintf(buf, MAX - 1, fmt, ap);
	if (iserror)
		snprintf(buf + strlen(buf), MAX - strlen(buf) - 1,
			": %s", strerror(error));
	strcat(buf, "\n");

	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
}


void err_ret(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}

void err_msg(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
}

void err_cont(int error, const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
}

void err_sys(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void err_exit(int error, const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void err_dump(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	abort();
	exit(EXIT_FAILURE);
}

void err_quit(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}
