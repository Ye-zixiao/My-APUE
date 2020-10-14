/**
 * 一个面向无连接的服务进程：
 * 使用前需要在/etc/services文件中添加ruptime   6666/udp字段
 * 以保证服务进程和客户进程能够从/etc/hosts和/etc/services中获取
 * 到应该运行在那个端口，目标进程的地址又在哪里
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <netdb.h>
#include <syslog.h>

#define BUFLEN 128
#define MAXADDRLEN 256

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif


void serve(int sockfd) {
	int n; socklen_t alen; FILE* fp;
	char buf[BUFLEN], abuf[MAXADDRLEN];
	struct sockaddr* addr = (struct sockaddr*)abuf;

	set_cloexec(sockfd);
	for (;;) {
		alen = MAXADDRLEN;
		/* 先执行recvfrom操作的目的是为了从客户进程发送过来的消息中提取地址等相关信息 */
		if ((n = recvfrom(sockfd, buf, BUFLEN, 0, addr, &alen)) == -1) {
			syslog(LOG_ERR, "ruptimed: recvfrom error: %s",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
			sprintf(buf, "error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		else {
			if (fgets(buf, BUFLEN, fp) != NULL)
				sendto(sockfd, buf, strlen(buf), 0, addr, alen);
			pclose(fp);
		}
	}
}


int main(int argc, char* argv[])
{
	struct addrinfo* ailist;
	struct addrinfo hint;
	int sockfd, err, n;
	char* host;

	if (argc != 1)
		err_quit("usage: ruptimed");

	if ((n = sysconf(_SC_HOST_NAME_MAX)) == -1)
		n = HOST_NAME_MAX;
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
	if (gethostname(host, n) == -1)
		err_sys("gethostname error");

	daemonize("ruptimed");
	memset(&hint, 0, sizeof(hint));
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_CANONNAME;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s", gai_strerror(err));
		exit(EXIT_FAILURE);
	}

	for (struct addrinfo* aip = ailist; aip != NULL; aip = aip->ai_next) {
		//这里init_server只做socket和bind操作
		if ((sockfd = init_server(SOCK_DGRAM, aip->ai_addr,
				aip->ai_addrlen, 0)) >= 0) {
			serve(sockfd);
			exit(EXIT_SUCCESS);
		}
	}
	exit(EXIT_FAILURE);
}
