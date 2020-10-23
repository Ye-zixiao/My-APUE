#include "open.h"
#include <string.h>

#define BUFFSIZE 8192

int main(int argc, char* argv[])
{
	int fd, nread;
	char buf[BUFFSIZE];
	char line[MAXLINE];

	if (setvbuf(stdout, NULL, _IOLBF, 0) != 0)
		err_sys("setvbuf error");

	printf("prompt> ");
	while (fgets(line, MAXLINE, stdin) != NULL) {
		/* 对读入的字符串进行预处理然后试图使用csopen()
			函数请求服务子进程打开指定的文件 */
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';
		if ((fd = csopen(line, O_RDONLY)) < 0)
			goto prompt;

		while ((nread = read(fd, buf, BUFFSIZE)) > 0)
			if (write(STDOUT_FILENO, buf, nread) != nread)
				err_sys("write error");
		if (nread < 0)
			err_sys("read error");
		close(fd);

prompt:
		printf("\nprompt> ");
	}

	exit(EXIT_SUCCESS);
}


/*
 试验结果：
$ ./client
prompt> strtok.c 0
/**
 * 我们的目的仍然是从上的字符串中"hello\tworld show\nme+your-code"
 * 分割出子字符串单词
 */
#include <stdio.h>
#include <string.h>

int main(void){
    char str[64] = "hello\tworld show\nme+your-code";
    char delim[6] = " \t\n-+";
    char *substr = NULL;

    substr = strtok(str, delim);
    if(substr == NULL)
        return 0;
    printf("%s\n", substr);
    while((substr = strtok(NULL, delim)) != NULL){
        printf("%s\n",substr);
    }

    return 0;
}

prompt> 

 */
