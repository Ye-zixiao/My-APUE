/*
	程序在不调用exit或return的时候，其父进程收到的终止状态为其printf打印的字符数
*/

#include<stdio.h>
#include<string.h>

void main(void)
{
	const char* msg = "helloworldshowmethecode\n";
	printf("strlen= %ld\n", strlen(msg));
	printf("%s", msg);
}