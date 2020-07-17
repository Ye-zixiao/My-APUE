#include"apue.h"

int main(void)
{
	int ch;
	while((ch=getc(stdin))!=EOF)
		if(putc(ch,stdout)==EOF)
			err_sys("output error");
	if(ferror(stdin))
		err_sys("input error");

	exit(0);
}
