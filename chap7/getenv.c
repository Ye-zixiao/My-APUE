/*
* 支持多个小写字符串查询环境变量值的程序
*/

#include"MyAPUE.h"
#include<ctype.h>
#include<string.h>

void StringUpper(char* buf, int buf_size) {
	int i;
	for (i = 0; buf[i] != '\0' || i < buf_size; ++i)
		buf[i] = toupper(buf[i]);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage:getenv <env>\n");

	int i = 1;
	const char* penv_value;
	char string[64];
	for (; i < argc; ++i) {
		if (strcpy(string, argv[i]) == NULL)
			continue;
		StringUpper(string, 64);
		if ((penv_value = getenv(string)) == NULL)
			continue;
		printf("%s=\n%s\n", string, penv_value);
	}

	exit(EXIT_SUCCESS);
}