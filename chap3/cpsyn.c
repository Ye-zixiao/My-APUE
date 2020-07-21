#include"apue.h"
#include"Setfsf.h"
#include<fcntl.h>

#define BUFSIZE 4096

int main(void)
{
	int n;
	char buf[BUFSIZE];
	set_fsflag(STDOUT_FILENO, O_SYNC);
	
	//检测O_SYNC文件状态标志是否被设置
	if((n=fcntl(STDOUT_FILENO,F_GETFL,0))<0)
		err_sys("fcntl error for fd STDOUT_FILENO");
	if(n&O_SYNC)printf("synchronous writes");
	else printf("synchronous write isn't valid\n");

	while ((n = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");
	if (n < 0)
		err_sys("read error");
	exit(0);
}
