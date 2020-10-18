/**
 * 将一个文件路径与新建的UNIX域套接字进行绑定
 */
#include "../include/MyAPUE.h"
#include <sys/un.h>

int main(int argc, char* argv[])
{
	struct sockaddr_un unaddr;
	socklen_t size;
	int sockfd;

	if (argc != 2)
		err_quit("usage: mkunsock <unix-sock-name>");

	unaddr.sun_family = AF_UNIX;
	strcpy(unaddr.sun_path, argv[1]);
	size = offsetof(struct sockaddr_un, sun_path) + strlen(unaddr.sun_path);
	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	if (bind(sockfd, (struct sockaddr*)&unaddr, size) == -1)
		err_sys("bind error");
	printf("UNIX domain socket bound to %s\n", argv[1]);
	close(sockfd);

	exit(EXIT_SUCCESS);
}
