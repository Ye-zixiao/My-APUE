#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

bool is_unbuffered(FILE* fp) {
	return fp->_flags & _IONBF;
}

bool is_linebuffered(FILE* fp) {
	return fp->_flags & _IOLBF;
}

bool is_fullbuffered(FILE* fp) {
	return fp->_flags & _IOFBF;
}

int buffer_size(FILE* fp) {
	return fp->_IO_buf_end -
		fp->_IO_buf_base;
}

void print_iostat(const char* stream, FILE* fp) {
	printf("%s: ", stream);
	if (fp != NULL) {
		if (is_unbuffered(fp))
			printf("nonbuffered");
		else if (is_linebuffered(fp))
			printf("line buffered");
		else
			printf("full buffered");
		printf(", buffer size: %d\n", buffer_size(fp));
	}
	else
		printf("stream invalid\n");
}