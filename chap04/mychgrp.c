#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc != 3)
		err_sys("Usage:mychgrp new_grpid filename\n");
	if (lchown(argv[2], -1, atoi(argv[1])) < 0)
		err_sys("change group id error\n");

	exit(0);
}