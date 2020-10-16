/**
 * TCPClient.c
 * 一个基于流式套接字(TCP)的客户进程，将字符串数据发送给服务进程，
 * 然后从中获取结果
 * 
 */
#include "../include/MyAPUE.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 128
#define PORT_NUM 12000


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	char buf[MAXLINE];
	int sockfd;

	if (argc != 2)
		err_quit("usage: tcp_client <server addr>");

	/* 客户端尝试与服务进程进行连接 */
	memset(&svaddr, 0, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(PORT_NUM);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("server address convert error");
	if ((sockfd = connect_retry(AF_INET, SOCK_STREAM, 0,(struct sockaddr*)&svaddr,
							sizeof(struct sockaddr_in))) == -1)
		err_sys("connect_retry error");

	printf("Input: "); fflush(stdout);
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
