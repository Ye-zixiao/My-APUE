#ifndef _MY_AUPE_H_
#define _MY_APUE_H_

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include<stdio.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#define BUFSIZE 4096

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}


#endif