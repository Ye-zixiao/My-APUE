/**
 * 客户进程的工作：
 * 1、使用getaddrinfo()函数获取目标服务(主机)的地址、端口号、
 *    协议类型等信息（它会到/etc/hosts和/etc/services这两个
 *    文件查找）
 *
 * 2、然后使用connect_retry()：
 *                       socket()  创建套接字
 *                   --->connect() 创建到服务套接字的逻辑连接
 *
 * 3、最后调用recv()函数从服务进程获取相应的信息并打印
 * */
#include "../include/MyAPUE.h"
#include <netdb.h>
#include <string.h>

#define BUFLEN 128


void print_uptime(int sockfd) {
	int nrecv;
	char buf[BUFLEN];

	while ((nrecv = recv(sockfd, buf, BUFLEN, 0)) > 0)
		write(STDOUT_FILENO, buf, nrecv);
	if (nrecv < 0)
		err_sys("recv error");
}


int main(int argc, char* argv[])
{
	struct addrinfo* ailist, * aip;
	struct addrinfo hint;
	int sockfd, err;

	if (argc != 2)
		err_quit("usage: ruptime <hostname>");

	/* 使用getaddrinfo函数根据给定主机名获取目标服务进程
	 * 的地址信息 */
	memset(&hint, 0, sizeof(hint));
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0,
									aip->ai_addr, aip->ai_addrlen)) == -1)
			err = errno;
		else {
			print_uptime(sockfd);
			exit(EXIT_SUCCESS);
		}
	}
	err_exit(err, "can't connect to %s", argv[1]);
}

/*
实验方法：

1、首先需要在/etc/services文件中加入如下的一条信息：
		ruptime    66666/tcp
   #该信息使得服务进程能够准确的知道自己应该运行的端口和协议类型
2、然后运行如下命令，结果如下所示：
# yexinhai @ Localhost in ~/My-APUE/chap16 on git:master x [19:34:01]
$ ./ruptime_tcpserver

# yexinhai @ Localhost in ~/My-APUE/chap16 on git:master x [19:34:03]
$ ./ruptime_tcpclient Localhost
 19:34:15 up  4:46,  1 user,  load average: 0.13, 0.12, 0.09

此时服务进程运行在IP地址为Localhost主机名所映射的地址上，端口号为66666；
而为了能够使客户进程能够找到服务进程所绑定的IP地址，需要用户主动的输入
Localhost或者你也可以在/etc/hosts文件中直接查看对应的IP地址，在我的电脑上为
127.0.1.1，所以我们也可以使用如下命令实验：
$ ./ruptime_tcpclient 127.0.1.1
结果也是相同的

 */
