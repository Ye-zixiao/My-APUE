#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void)
{
	char buf[64];
	if (gethostname(buf, 64) != 0) {
		fprintf(stderr, "gethostname error\n");
		exit(EXIT_FAILURE);
	}
	printf("hostname: %s\n", buf);

	exit(EXIT_SUCCESS);
}