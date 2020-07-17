#include<stdio.h>
#include<limits.h>
#include<unistd.h>
#include"apue.h"
#include<sys/resource.h>

#define OPEN_MAX_GUESS 256

long open_max(void);

int main(int argc,char*argv[])
{
	printf("open_max=%ld\n",
			open_max());
	return 0;
}

long open_max(void){
	long openmax;
	struct rlimit r1;

	if((openmax=sysconf(_SC_OPEN_MAX))<-1||
			openmax==LONG_MAX){
		if(getrlimit(RLIMIT_NOFILE,&r1))
			err_sys("can't get file limit");
		if(r1.rlim_max==RLIM_INFINITY)
			openmax=OPEN_MAX_GUESS;
		else
			openmax=r1.rlim_max;	
	}
	return openmax;
}
