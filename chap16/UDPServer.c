/**
 * UDPServer.c
 * 一个基于数据报(协议为UDP)的服务进程，从客户端获取数据(也同时需要
 * 截取地址端口等信息)，然后将其中的小写字符串转换为大写字符串后发送
 * 给客户端进程
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
	char buf[MAXLINE], claddrStr[INET_ADDRSTRLEN];
	struct sockaddr_in svaddr, claddr;
	int sockfd, nrecv;
	socklen_t alen;

	if (argc != 1)
		err_quit("usage: UDPServer");
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		err_sys("signal error");

	/* 初始化基于UDP的服务器进程 */
	memset(&svaddr, 0, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = INADDR_ANY;
	svaddr.sin_port = htons(PORT_NUM);
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	if (bind(sockfd, (struct sockaddr*)&svaddr, sizeof(struct sockaddr_in)) == -1)
		err_sys("bind error");

	for (;;) {
		/* 接收消息并获取地址端口等信息，并打印这些信息 */
		alen = sizeof(struct sockaddr_in);
		if ((nrecv = recvfrom(sockfd, buf, MAXLINE, 0,
							(struct sockaddr*)&claddr, &alen)) == -1) {
			err_msg("recvfrom error");
			continue;
		}
		if (inet_ntop(AF_INET, &claddr.sin_addr, claddrStr, INET_ADDRSTRLEN) == NULL)
			err_sys("inet_ntop error");
		printf("Received new datagram from %s:%hu...\n", claddrStr, claddr.sin_port);

		for (int i = 0; i < nrecv; ++i)
			buf[i] = toupper(buf[i]);
		if (sendto(sockfd, buf, nrecv, 0, (struct sockaddr*)&claddr,
					sizeof(struct sockaddr_in)) == -1)
			err_sys("sendto error");
	}
}
