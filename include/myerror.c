#include"MyAPUE.h"

void err_sys(const char* msg) {
	fputs(msg, stderr);
	exit(EXIT_FAILURE);
}