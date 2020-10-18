#include "../include/MyAPUE.h"
#include <sys/socket.h>
#include <sys/un.h>

#define MAXSLEEP 128

/**
 * 建立与服务进程的Internet套接字的逻辑连接，并在失败时以指数补
 * 偿算法进行重试
 */
int connect_retry(int domain, int type, int protocol,
	const struct sockaddr* addr, socklen_t alen) {
	int numsec, fd;

	for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
		if ((fd = socket(domain, type, protocol)) == -1)
			return -1;
		if (connect(fd, addr, alen) == 0)
			return fd;
		close(fd);

		if (numsec <= MAXSLEEP / 2)
			sleep(numsec);
	}
	return -1;
}

/**
 * 完成Internet流式套接字/数据报套接字服务进程初始化一条龙，并且调用
 * setsockopt()使得进程在主机重启之后仍然能够重新绑定该地址
 */
int init_server(int type, const struct sockaddr* addr,
				socklen_t alen, int qlen) {
	int fd, err = 0;
	int reuse = 1;

	if ((fd = socket(addr->sa_family, type, 0)) < 0)
		return -1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
		goto errout;
	if (bind(fd, addr, alen) < 0)
		goto errout;
	if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
		if (listen(fd, qlen) < 0)
			return -1;
	}
	return fd;

errout:
	err = errno;
	close(fd);
	errno = err;
	return -1;
}


#define QLEN 10
/**
 * 创建一个服务器连接端点，完成socket---->bind----->listen流程，宣告
 * 在一个与指定文件路径（一个众所周知的名字）绑定的套接字上监听客户进
 * 程的连接请求，并在调用成功后返回与之关联的UNIX套接字描述符
 */
static int server_listen(const char* name, int qlen) {
	struct sockaddr_un unaddr;
	int                sockfd, rval, err;
	socklen_t          alen;

	if (strlen(name) >= sizeof(unaddr.sun_path)) {
		errno = ENAMETOOLONG;
		return -1;
	}
	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		return -2;

	memset(&unaddr, 0, sizeof(unaddr));
	unaddr.sun_family = AF_UNIX;
	strcpy(unaddr.sun_path, name);
	alen = offsetof(struct sockaddr_un, sun_path) + strlen(name);
	unlink(name);
	if (bind(sockfd, (struct sockaddr*)&unaddr, alen) == -1) {
		rval = -3;
		goto errout;
	}
	if (listen(sockfd, qlen) == -1) {
		rval = -4;
		goto errout;
	}
	return sockfd;

errout:
	err = errno;
	close(sockfd);
	errno = err;
	return rval;
}


int serv_listen(const char* name) {
	return server_listen(name, QLEN);
}


#define STALE 30		//不允许客户进程关联的socket文件旧于该STALE秒之前
/**
 * 当服务进程接收到用户进程的连接请求，则可以通过调用本函数进行请求接收，
 * 并创建出一个新的UNIX域套接字负责与客户进程进行数据传送
 */
int serv_accept(int listenfd, uid_t* uidptr) {
	struct sockaddr_un   unaddr;
	int                  clientfd, err, rval;
	struct stat          statbuf;
	time_t               staletime;
	socklen_t            alen;
	char*                name;

	alen = sizeof(unaddr);
	if ((name = malloc(sizeof(unaddr.sun_path) + 1)) == NULL)
		return -1;
	if ((clientfd = accept(listenfd, (struct sockaddr*)&unaddr, &alen)) == -1) {
		free(name);
		return -2;
	}

}
