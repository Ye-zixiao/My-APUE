/**
 * 一个面向无连接的ruptime客户进程
 * */
#include "../include/MyAPUE.h"
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

//#define DEBUG
#define BUFLEN 128
#define TIMEOUT 20

void sigalrm(int signo) {}

void print_uptime(int sockfd, struct addrinfo* aip) {
	int nrecv;
	char buf[BUFLEN];

	buf[0] = '\0';
#ifdef DEBUG
	char temp[BUFSIZE];
	printf("addr: %s\n", inet_ntop(aip->ai_family, aip->ai_addr, temp, BUFSIZE));
#endif
	if (sendto(sockfd, buf, 1, 0, aip->ai_addr, aip->ai_addrlen) < 0)
		err_sys("sendto error");

	/* alarm()函数的作用是防止recvfrom函数长时间阻塞 */
	alarm(TIMEOUT);
	if ((nrecv = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
		if (errno != EINTR)
			alarm(0);
		err_sys("recv error");
	}
	alarm(0);
	write(STDOUT_FILENO, buf, strlen(buf));
}

int main(int argc, char* argv[])
{
	struct addrinfo* ailist;
	struct addrinfo hint;
	struct sigaction sa;
	int sockfd, err;

	if (argc != 2)
		err_quit("usage: ruptime hostname");
	
	//设置信号处理程序
	sa.sa_flags = 0;
	sa.sa_handler = sigalrm;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		err_sys("sigaction error");

	//根据给定主机名获取地址相关信息
	memset(&hint, 0, sizeof(hint));
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) == -1)
		err_quit("getaddrinfo error: %s", gai_strerror(err));

	//创建套接字直接使用sendto函数发送消息并使用recvfrom等待回应
	for (struct addrinfo* aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) < 0)
			err = errno;
		else {
			print_uptime(sockfd, aip);
			exit(EXIT_SUCCESS);
		}
	}

	fprintf(stderr, "can't contact %s: %s\n", argv[1], strerror(err));
	exit(EXIT_FAILURE);
}
