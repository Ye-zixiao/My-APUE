/*
 *	这个程序主要是用来测试一个有创建文件大小限制的进程若向其
 *	写入超过该软限制的大小的文件时会发生什么
 *
 * */
#include"../include/MyAPUE.h"
#include<fcntl.h>

#define BUFFSIZE 100
#define DEFAULT_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

static void signal_intr(int signo) {
	fputs("received signal SIGXFSZ\n", stderr);
}


//使用时需要使用重定向指定输入文件
int main(void)
{
	struct rlimit limitbuf;
	char charbuf[BUFFSIZE];
	int fd, val, ret;

	//信号注册并设置最大生成文件资源限制
	if (signal(SIGXFSZ, signal_intr) == SIG_ERR)
		err_sys("signal error\n");
	if (getrlimit(RLIMIT_FSIZE, &limitbuf) != 0)
		err_sys("getrlimit error\n");
	limitbuf.rlim_cur = 1024;
	if (setrlimit(RLIMIT_FSIZE, &limitbuf) != 0)
		err_sys("setrlimit error\n");
	pr_limit(RLIMIT_FSIZE);

	//复制文件
	if ((fd = creat("test.file", DEFAULT_MODE)) == -1)
		err_sys("creat error\n");

	 /*		在实际中，Linux并不会因为进程写入的数据达到进程资源限
	  *	制的软限制值而发生SIGXFSZ信号，它只是让write系统调用部分成
	  *	功，并返回最后一个写入的数据量
 	  */
	while((val=read(STDIN_FILENO,charbuf,BUFFSIZE))>0)
		if ((ret = write(fd, charbuf, val)) != val) {
			fprintf(stderr, "write function actually write %d chars, but charbuf has %d chars want to output\n",
				ret, val);
			break;
		}
	//若在文件满的情况下再向文件中写入数据，就会真的会导致SIGXFSZ信号的发生
	if((val=read(STDIN_FILENO,charbuf,BUFFSIZE))>0)
		if(write(fd,charbuf,val)!=val)
			fprintf(stderr,"another try failed\n");

	close(fd);
	exit(EXIT_SUCCESS);
}
