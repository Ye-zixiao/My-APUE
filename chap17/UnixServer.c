/**
 * UnixServer.c：
 * 一个接收用户进程传送过来的数据，然后进行简单的打印输出
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <pwd.h>

#define SERVER_PATH "/tmp/unix_server"

void sig_term(int signo) {
	unlink(SERVER_PATH);
	printf("Socket file(%s) has been removed\n", SERVER_PATH);
	exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
	int svfd, clifd, nread;
	char buf[BUFSIZE];
	uid_t uid;

	if (signal(SIGTERM, sig_term) == SIG_ERR)
		err_sys("signal error");
	if ((svfd = server_listen(SERVER_PATH, 1)) < 0)
		err_sys("server_listen error");

	for (;;) {
		if ((clifd = serv_accept(svfd, &uid)) < 0)
			err_sys("serv_accept error");
		printf("Accept new connection from %s...\n", getusername(uid));

		while ((nread = read(clifd, buf, BUFSIZE)) > 0) {
			if (write(STDOUT_FILENO, buf, nread) != nread)
				err_sys("write error");
		}
		putchar('\n');
		if (nread < 0)
			err_sys("read error");
		close(clifd);
	}
}
