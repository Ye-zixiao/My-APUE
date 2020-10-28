/**
 * 一个不会使服务进程阻塞在等待子进程执行完毕才能继续
 * 处理后续请求的uptime服务进程
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


static void sig_child(int signo) {
	while (waitpid((pid_t)-1, NULL, WNOHANG) > 0);
}


/**
 * 使用SA_RESTART标志调用sigaction()函数进行信号处理程序注册，
 * 这样因子进程终止而产生的信号SIG_CHLD即使中断服务进程的系统
 * 调用正常执行，仍然能够重新启动
 */
Sigfunc* signal_restart(int signo, Sigfunc* func) {
	struct sigaction act, oact;
	
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_flags |= SA_RESTART;
	if (sigaction(signo, &act, &oact) == -1)
		return SIG_ERR;
	return oact.sa_handler;
}


void server(int sockfd) {
	int clfd;
	pid_t pid;

	if (signal_restart(SIGCHLD, sig_child) == SIG_ERR)
		syslog(LOG_ERR, "signal_restart error");

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
		else
			close(clfd);
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
