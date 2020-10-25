#include"apue.h"

int main(int argc,char*argv[])
{
	int i = 0;
	for(;i<argc;++i)
		printf("ARG[%d]: %s\n",i,argv[i]);

	exit(EXIT_SUCCESS);
}
