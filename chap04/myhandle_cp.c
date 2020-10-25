/*
* 可以处理源文件中不想要的字符的拷贝程序
* 默认情况下，使用myhandle_cp [srcfile] [tarfile] 会删除源文件中的空洞
* 在加入第四个参数后，使用myhandle_cp [srcfile] [tarfile] [你不想要的字符] 会删除源文件中的你不想要的字符，
* 然后拷贝到目标文件中
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>

#define BUFSIZE 4096
#define DEFAULT_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

/*
* 输入输出缓冲区字符处理函数：
* buf指向缓冲区，read_size表示前面的read函数读取了多少有效的字符放入到buf中，
* ch表示希望从buf中删除的字符
*/
int buf_handle(char* buf, int read_size, char ch) {
	int i;
	for (i = 0; i < read_size; ++i) {
		if (buf[i] == ch) {
			int j = i + 1;
			for (; j < read_size; ++j)
				if (buf[j] != ch)
					buf[i++] = buf[j];
			break;
		}
	}
	return i;
}

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	if (argc < 3)
		err_sys("usage:myhandle_cp [src file] [target file] [del_char]\n");
	char ch = '\0';
	if (argc == 4)ch = argv[3][0];

	int val, src_fd, tar_fd;
	char buf[BUFSIZE];
	if ((src_fd = open(argv[1], O_RDONLY)) < 0 ||
		(tar_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, DEFAULT_MODE)) < 0)
		err_sys("open error\n");

	while ((val = read(src_fd, buf, BUFSIZE)) > 0) {
		val = buf_handle(buf, val, ch);
		if (write(tar_fd, buf, val) != val)
			err_sys("write error\n");
	}
	if (val < 0)
		err_sys("read error\n");
	exit(EXIT_SUCCESS);
}