#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(void)
{
	printf("ruid = %d, euid = %d\n", getuid(), geteuid());

	exit(0);
}