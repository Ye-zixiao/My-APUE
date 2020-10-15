/**
 * 使用TCP的小写转大写服务端进程
 */
#include "../include/MyAPUE.h"
#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 128

int main(int argc, char* argv[]) {
	struct sockaddr_in addr;
	struct sockaddr clientaddr;
	char buf[MAXLINE], addrbuf[INET_ADDRSTRLEN];
	int sockfd, clfd, nrecv;
	socklen_t slen;

	if (argc != 1)
		err_sys("usage: server");

	/* 初始化服务进程 */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;//接收任一连接
	addr.sin_port = 12000;
	if ((sockfd = init_server(SOCK_STREAM, (struct sockaddr*)&addr, sizeof(addr), 1)) == -1)
		err_sys("init_server error");

	for (;;) {
		/* 接收连接请求并生成新的套接字负责与之连接 */
		slen = (socklen_t)sizeof(clientaddr);
		if ((clfd = accept(sockfd, &clientaddr, &slen)) == -1)
			err_sys("accept error");
		printf("Accept new connect from: %s\n",
						inet_ntop(AF_INET, &clientaddr, addrbuf, sizeof(addrbuf)));

		while ((nrecv = recv(clfd, buf, MAXLINE, 0)) > 0) {
			for (int i = 0; i < strlen(buf); ++i)
				buf[i] = toupper(buf[i]);
			buf[strlen(buf) - 1] = '\0';
			if (send(clfd, buf, strlen(buf), 0) == -1)
				err_sys("send error");
		}
		if (nrecv < 0)
			err_sys("recv error");
		close(clfd);
	}
}
