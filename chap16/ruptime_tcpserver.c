/**
 * 服务进程的工作：
 * 1、使用gethostname()获取主机名字
 * 
 * 2、然后使用getaddrinfo()根据主机名字获取主机应该工作的地址、
 *    端口、协议类型等信息（它回到/etc/hosts和/etc/services两个
 *    文件中查找）
 * 
 * 3、然后调用serve()：   
 *                   socket()   创建套接字
 *              --->  bind()    与IP地址绑定
 *	            ---> listen()   宣告当前进程会监听网络请求
 * 
 * 4、最后服务进程可以调用accept()接收来自其他进程的网络请求，成
 *    功则会创建一个新的套接字描述符与客户进程的套接字形成逻辑连接
 * 
 * 5、最后将相应的信息发送给客户进程
 * 
 */
#include "../include/MyAPUE.h"
#include <netdb.h>
#include <string.h>
#include <syslog.h>
#include <arpa/inet.h>

#define BUFLEN 128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif


void serve(int sockfd) {
	int clfd;
	FILE* fp;
	char buf[BUFLEN];

	//生成子进程时需要自动关闭那些继承而来的文件描述符
	set_cloexec(sockfd);
	for (;;) {
		/* 接受一个连接请求然后创建一个文件（套接字）描述符与客户进程相连 */
		if ((clfd = accept(sockfd, NULL, NULL)) < 0) {
			syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		set_cloexec(clfd);
		//if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
		if ((fp = popen("ps -aux|wc -l", "r")) == NULL) {//返回给客户进程当前运行进程数
			sprintf(buf, "error: %s\n", strerror(errno));
			send(clfd, buf, strlen(buf), 0);
		}
		else {
			while (fgets(buf, BUFLEN, fp) != NULL)
				send(clfd, buf, strlen(buf), 0);
			pclose(fp);
		}
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

	if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX;
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
	if (gethostname(host, n) < 0)
		err_sys("gethostname error");

	daemonize("ruptimed");
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s", gai_strerror(err));
		exit(EXIT_FAILURE);
	}
	for (struct addrinfo* aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = init_server(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) {
			serve(sockfd);
			exit(EXIT_SUCCESS);
		}
	}
	
	exit(EXIT_FAILURE);
}
