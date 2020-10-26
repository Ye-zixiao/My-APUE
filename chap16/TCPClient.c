/**
 * TCPClient.c
 * 一个基于流式套接字的客户进程，将用户输入的字符串发送给
 * 服务进程，然后从服务进程中接收到响应信息并输出
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 256

int main(int argc, char* argv[])
{
	char buf[MAXLINE], addr[INET_ADDRSTRLEN];
	struct sockaddr_in svaddr;
	in_port_t port;
	int sockfd;
	int ch;

	opterr = 0;
	while ((ch = getopt(argc, argv, "hp:a:")) != -1) {
		switch(ch) {
		case 'h':
			err_quit("usage: TCPClient [-p port] [-a ipv4-address]\n"
		             "       TCPClient [-h]");
		case 'p':
			port = (in_port_t)atoi(optarg);
			break;
		case 'a':
			strncpy(addr, optarg, INET_ADDRSTRLEN - 1);
			addr[INET_ADDRSTRLEN - 1] = 0;
			break;
		case '?':
			err_quit("unrecognized option: -%c", optopt);
		}
	}

	/* 客户进程试图与服务进程进行连接 */
	memset(&svaddr, 0, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(port);
	if (inet_pton(AF_INET, addr, &svaddr.sin_addr) <= 0)
		err_sys("server address convert error");
	if ((sockfd = connect_retry(AF_INET, SOCK_STREAM, 0,
			(struct sockaddr*)&svaddr, sizeof(struct sockaddr_in))) == -1)
		err_sys("connect_retry error");

	printf("Input: "); fflush(stdout);
	if (fgets(buf, MAXLINE, stdin) == NULL)
		err_sys("fgets error");
	if (send(sockfd, buf, strlen(buf) + 1, 0) == -1)
		err_sys("send error");
	if (recv(sockfd, buf, MAXLINE, 0) == -1)
		err_sys("recv error");
	printf("Server: %s", buf);

	close(sockfd);
	exit(EXIT_SUCCESS);
}
