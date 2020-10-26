/**
 * TCPServer.c
 * 一个基于流式套接字(协议为TCP)，将接收到的小写字符串转换为大写字符串
 * 并回复给客户进程的服务器进程
 * 
 */
#include "../include/MyAPUE.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 256
#define PORT_NUM 12000


int main(int argc, char* argv[])
{
	char claddrStr[INET_ADDRSTRLEN], buf[MAXLINE];
	struct sockaddr_in svaddr, claddr;
	int sockfd, clfd, nrecv;
	socklen_t alen;

	if (argc != 1)
		err_quit("usage: tcp_server");
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		err_sys("signal error");

	/* 初始化服务进程，可接受来自任一地址的连接 */
	memset(&svaddr, 0, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = INADDR_ANY;
	svaddr.sin_port = htons(PORT_NUM);
	if ((sockfd = init_server(SOCK_STREAM, (struct sockaddr*)&svaddr,
							sizeof(struct sockaddr_in), 1)) == -1)
		err_sys("init_server error");

	for (;;) {
		alen = sizeof(struct sockaddr_in);
		if ((clfd = accept(sockfd, (struct sockaddr_in*)&claddr, &alen)) == -1) {
			err_msg("accept error");
			continue;
		}
		//转换为人易读的格式并打印
		if (inet_ntop(AF_INET, &claddr.sin_addr, claddrStr, INET_ADDRSTRLEN) == NULL)
			err_sys("inet_ntop error");
		printf("Accept new connect from %s:%hu...\n", claddrStr, claddr.sin_port);

		while ((nrecv = recv(clfd, buf, MAXLINE, 0)) > 0) {
			for (int i = 0; i < nrecv; i++)
				buf[i] = toupper(buf[i]);
			if (send(clfd, buf, nrecv, 0) == -1)
				err_sys("send error");
		}
		if (nrecv < 0)
			err_sys("recv error");
		close(clfd);
	}
}
