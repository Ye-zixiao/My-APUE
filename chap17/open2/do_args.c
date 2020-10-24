#include "opend.h"
#include <string.h>


#define WHITE " \t\n"//分隔符
#define MAXARGC 50


/**
 * 检查由buf_args()函数分割得到的参数列表是否正确，若正确则将对应参数
 * 用来设置pathname和oflag
 */
static int cli_args(int argc, char* argv[]) {
	if (argc != 3 || strcmp(argv[0], CL_OPEN) != 0) {
		strcpy(errmsg, "usage: <pathname>\n");
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
	int argc = 0;

	if (strtok(buf, WHITE) == NULL)
		return -1;
	argv[argc++] = buf;
	while ((ptr = strtok(NULL, WHITE)) != NULL) {
		if (argc >= MAXARGC - 1)
			return -1;
		argv[argc++] = ptr;
	}

	return (*optfunc)(argc, argv);
}


/**
 * 使用buf指向的字符数组中的信息打开指定文件，并将其文件描述符
 * 发送给客户进程 
 */
void handle_request(char* buf, int nread, int clifd, uid_t uid) {
	int newfd;

	if (buf[nread - 1] != '\0') {
		snprintf(errmsg, MAXLINE - 1, "request from uid %d not null terminated: %*.*s\n",
			uid, nread, nread, buf);
		send_err(clifd, -1, errmsg);
		return;
	}
	log_msg("request: %s, from uid %d", buf, uid);
	if (buf_args(buf, cli_args) < 0) {
		send_err(clifd, -1, errmsg);
		log_msg(errmsg);
		return;
	}

	if ((newfd = open(pathname, oflag)) < 0) {
		snprintf(errmsg, MAXLINE - 1, "can't open %s: %s\n",
			pathname, strerror(errno));
		send_err(clifd, -1, errmsg);
		log_msg(errmsg);
		return;
	}
	if (send_fd(clifd, newfd) < 0)
		log_sys("send_fd error");
	log_msg("send fd %d over fd %d for %s", newfd, clifd, pathname);
	close(newfd);
}