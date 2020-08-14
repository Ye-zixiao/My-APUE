#include"../include/MyAPUE.h"
#include<string.h>

void err_sys(const char* msg) {
	fputs(msg, stderr);
	exit(EXIT_FAILURE);
}

void err_exit(int err, const char* msg) {
	fprintf(stderr, "%s: %s\n", msg, strerror(err));
	exit(EXIT_FAILURE);
}
