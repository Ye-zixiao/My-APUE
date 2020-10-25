#include"apue.h"
#include<errno.h>

void make_temp(char* template);


int main(int argc, char* argv[])
{
	char good_template[] = "/tmp/fileXXXXXX";
	/*因为mkstemp函数会试图对template指针指向的字符数组中的占位符进行修改，
	所以这个template指针实参指向的字符数组空间必须存放在栈堆上，而不是这里的可执行文件只读段中*/
	char* bad_template = "/tmp/fileXXXXXX";
	
	printf("trying to create first temp file...\n");
	make_temp(good_template);
	printf("trying to create second temp file...\n");
	make_temp(bad_template);

	exit(EXIT_SUCCESS);
}

void make_temp(char* template) {
	int fd;
	struct stat statbuf;

	if ((fd = mkstemp(template)) < 0)
		err_sys("mkstemp error");
	printf("temp file name = %s\n", template);
	close(fd);

	//检查是否可stat
	if (stat(template, &statbuf) < 0) {
		if (errno == ENOENT)
			printf("file doesn't exist\n");
		else
			err_sys("stat fialed");
	}
	else {
		printf("file exist\n");
		unlink(template);
	}
}