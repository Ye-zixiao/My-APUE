/**
 * 使用TCP的小写转大写客户端进程
 */
#include "../include/MyAPUE.h"
#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 128

int main(int argc, char* argv[])
{
	struct sockaddr_in addr;
	char buf[MAXLINE];
	int sockfd;

	if (argc != 2)
		err_quit("usage: client <ip address>");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = 12000;
	if ((sockfd = connect_retry(AF_INET, SOCK_STREAM, 0, (struct sockaddr*)&addr, sizeof(addr))) == -1)
		err_sys("connect_retry error");
	
	printf("input: "); fflush(stdout);
	if (fgets(buf, MAXLINE, stdin) == NULL)
		err_sys("fgets error");
	if (send(sockfd, buf, strlen(buf) + 1, 0) == -1)
		err_sys("send error");
	if (recv(sockfd, buf, MAXLINE, 0) == -1)
		err_sys("recv error");
	printf("server: %s", buf);
	close(sockfd);

	exit(EXIT_SUCCESS);
}
