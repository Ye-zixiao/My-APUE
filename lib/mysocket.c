#include "../include/MyAPUE.h"
#include <sys/socket.h>

#define MAXSLEEP 128

/**
 * 建立与服务进程的套接字的逻辑连接，并在失败时以指数补偿算法
 * 进行重试
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
 * 使用socket--->bind(--->listen)一条龙初始化服务进程，服务器会将
 * 某一个套接字绑定到一个指定的地址上面，若是采用TCP还会调用listen
 * 函数宣告自己监听相关的端口
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
