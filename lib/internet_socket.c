#include "MyAPUE.h"
#include <sys/socket.h>

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
