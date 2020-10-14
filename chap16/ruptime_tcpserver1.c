/**
 * 在客户进程和服务进程之间创建完逻辑连接后：
 * 
 *     服务进程----------------------->运行uptime的子进程
 *        |                               |       | 
 *        |                               |       |
 *        v                               v       v
 *      socket()                 STDOUT_FILENO   STDERR_FILENO
 *    创建的sockfd                         \      /
 *   用来监听连接请求                        \   /
 *                                            |
 *                                            v
 *                                   accept()创建的套接字
 *                                            |
 *                                            |
 *                                            v
 *                        客户进程：   socket()创建的套接字
 */

#include "../include/MyAPUE.h"
#include <string.h>
#include <netdb.h>
#include <syslog.h>
#include <fcntl.h>

#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif


void server(int sockfd) {
	int clfd;
	pid_t pid;

	set_cloexec(sockfd);
	for (;;) {
		if ((clfd = accept(sockfd, NULL, NULL)) == -1) {
			syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if ((pid = fork()) < 0) {
			syslog(LOG_ERR, "ruptimed: fork error: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		else if (pid == 0) {
			/* 让执行uptime的子进程的stderr和stdout文件描述符指向accept
				创建的套接字，这样就避免了缓冲区和send这些步骤 */
			if (dup2(clfd, STDOUT_FILENO) != STDOUT_FILENO ||
				dup2(clfd, STDERR_FILENO) != STDERR_FILENO) {
				syslog(LOG_ERR, "ruptimed: unexpected error");
				exit(EXIT_FAILURE);
			}
			close(clfd);
			execl("/usr/bin/uptime", "uptime", (char*)NULL);
			syslog(LOG_ERR, "ruptimed: unexpected return from exec: %s",
				strerror(errno));
		}
		else {
			close(clfd);
			waitpid(pid, NULL, 0);
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
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) == -1) {
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s",
			gai_strerror(err));
		exit(EXIT_FAILURE);
	}
	for (struct addrinfo* aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = init_server(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen,
			QLEN)) >= 0) {
			server(sockfd);
			exit(EXIT_SUCCESS);
		}
	}

	exit(EXIT_FAILURE);
}
