#include"apue.h"

int main(void)
{
	if(lseek(STDIN_FILENO,0,SEEK_SET)==0)
		printf("cannot seek\n");
	else
		printf("seek ok\n");
	exit(0);	
}
