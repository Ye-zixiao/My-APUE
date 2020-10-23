#include "opend.h"
#include <string.h>

/**
 * WHITE使用用于strtok的分隔符，这里的分隔符的意思是：若针对的字符串中
 * 出现了分隔符字符串中的任一字符，则将针对字符串在此位置前后分割。例如
 * 将下面的WHITE适用于"hello world\tshow me\nyour code"分割成hello、
 * world、show、me、your、code
 */
#define WHITE " \t\n"
#define MAXARGC 50


/**
 * 检查由buf_args()函数分割得到的参数列表是否正确，若正确则将对应参数
 * 用来设置pathname和oflag
 */
static int cli_args(int argc, char* argv[]) {
	if (argc != 3 || strcmp(argv[0], CL_OPEN) != 0) {
		strcpy(errmsg, "usage: <pathname> <oflag>\n");
		return -1;
	}
	pathname = argv[1];
	oflag = atoi(argv[2]);
	return 0;
}


/**
 * 将buf指向的字符数组中的字符串进行分割，然后交给optfunc函数（实则
 * 是cli_args()函数）检查，并对pathname和oflag进行设置
 */
static int buf_args(char* buf, int (*optfunc)(int, char**)) {
	char* ptr, * argv[MAXARGC];
	int argc;

	if (strtok(buf, WHITE) == NULL)
		return -1;
	argv[argc = 0] = buf;
	while ((ptr = strtok(NULL, WHITE)) != NULL) {
		if (++argc >= MAXARGC - 1)
			return -1;
		argv[argc] = ptr;
	}

	return (*optfunc)(argc, argv);
}


/**
 * 使用buf指向的字符数组中的信息打开指定文件，并将其文件描述符
 * 发送给客户进程 
 */
void handle_request(char* buf, int nread, int fd) {
	int newfd;

	if (buf[nread - 1] != '\0') {
		snprintf(errmsg, MAXLINE - 1,
			"request not null terminated: %*.*s\n", nread, nread, buf);
		send_err(fd, -1, errmsg);
	}
	if (buf_args(buf, cli_args) == -1) {
		send_err(fd, -1, errmsg);
		return;
	}
	if ((newfd = open(pathname, oflag, FILE_MODE)) == -1) {
		snprintf(errmsg, MAXLINE - 1, "can't open %s: %s\n",
			pathname, strerror(errno));
		send_err(fd, -1, errmsg);
		return;
	}
	if (send_fd(fd, newfd) < 0)
		err_sys("send_fd error");
	close(newfd);
}