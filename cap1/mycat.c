#include"apue.h"
#define BUFFSIZE 4096

int main(void)
{
	int n;
	char buf[BUFFSIZE];

	while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0){//从标准输入中读取数据
		printf("read %d char\n",n);
		if(write(STDOUT_FILENO,buf,n)!=n)//将从标准输入中读取的数据写入到标准输出当中
			err_sys("write error");
	}

	//其中当最后一次向标准输出写入的数据量不到n时就会退出循环
	if(n<0)
		err_sys("read error");

	exit(0);	
}
