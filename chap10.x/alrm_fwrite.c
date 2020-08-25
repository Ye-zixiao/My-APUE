/*
		在Linux中以fwrite读写一个大的缓冲区时会直接调用write函数，
	在读写的过程中仿佛会对写入过程中发生的信号进行阻塞，即只有在读写
	完成之后才会递交SIGALRM信号。特别的，在我机械硬盘的Linux上实验
	确实如上所言，但是在我固态硬盘的电脑上就不是这样了，它会很快就完成😂
*/
#include"../include/MyAPUE.h"
#include<fcntl.h>

#define BUFFSIZE 1024*1024*256

static int buf[BUFFSIZE];

static void sig_alrm(int signo) {
	if (signo == SIGALRM)
		printf("received signal SIGALRM\n");
}


int main(void)
{
	FILE* pf;
	int ret;

	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal error\n");
	
	if((pf=fopen("test.file","w"))==NULL)
		err_sys("fopen error\n");
	alarm(1);
	if ((ret = fwrite(buf, sizeof(int), BUFFSIZE, pf)) != BUFFSIZE) {
		fprintf(stderr, "fwrite only write %d ints\n", ret);
	}
	alarm(0);
	printf("done\n");	

	fclose(pf);
	return 0;
}
