#include"../include/MyAPUE.h"
#include<string.h>

//这段代码现在好像不能编译了，Linux上我不太清楚为了什么找不到
//sys_siglist的定义，而且手册也更推荐使用strsignal来代替之
extern const char* const sys_siglist[];

int Sig2Str(int signo, char* str) {
	if (signo < 0 || signo >= _NSIG)
		return -1;
	strcpy(str, sys_siglist[signo]);
	return 0;
}

int main(void)
{
	char buf[SBUFSIZE];

	if (Sig2Str(SIGALRM, buf) != 0)
		err_sys("Sig2Str error\n");
	printf("SIGALRM ----> %s\n", buf);
	exit(EXIT_SUCCESS);
}
