/**
 * UDPClient.c
 * 一个基于数据报(协议为UDP)的客户进程，将数据发送给指定服务进程
 * 然后从其中接收相应的返回数据进行打印
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
		err_quit("usage: UDPClient <server addr>");
	
	memset(&svaddr, 0, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(PORT_NUM);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) == -1)
		err_sys("inet_pton error");
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	
	printf("Input: "); fflush(stdout);
	if (fgets(buf, MAXLINE, stdin) == NULL)
		err_sys("fgets error");
	if (sendto(sockfd, buf, strlen(buf) + 1, 0,
				(struct sockaddr*)&svaddr, sizeof(struct sockaddr_in)) == -1)
		err_sys("sendto error");
	if (recv(sockfd, buf, MAXLINE, 0) == -1)
		err_sys("recv error");
	printf("Server: %s", buf);
	close(sockfd);

	exit(EXIT_SUCCESS);
}
