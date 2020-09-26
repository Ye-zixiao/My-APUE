#include "../include/MyAPUE.h"
#include <fcntl.h>


/**
 * 获取/设置(注册)文件锁万能函数.
 * @param	fd		文件描述符
 * @param	cmd		fcntl记录锁获取/设置标志
 * @param	type	记录锁类型
 * @param	offset	相对于whence的起始位置
 * @param	whence	SEEK_SET、SEEK_END、SEEK_CUR
 * @param	len		文件加锁区长度
 * @return			是否成功
 */
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, int len) {
	struct flock flockbuf;

	flockbuf.l_type = type;
	flockbuf.l_whence = whence;
	flockbuf.l_start = offset;
	flockbuf.l_len = len;

	return fcntl(fd, cmd, &flockbuf);
}


/**
 * 测试文件记录锁是否被加锁
 * @param	fd		文件描述符
 * @param	cmd		fcntl记录锁获取/设置标志
 * @param	type	记录锁类型
 * @param	offset	相对于whence的起始位置
 * @param	whence	SEEK_SET、SEEK_END、SEEK_CUR
 * @param	len		文件加锁区长度
 * @return			返回0表示记录锁没有被其他进程持有，否则返回持有记录锁进程ID
 */
int lock_test(int fd, int cmd, int type, off_t offset, int whence, int len) {
	struct flock flockbuf;

	flockbuf.l_type = type;
	flockbuf.l_whence = whence;
	flockbuf.l_start = offset;
	flockbuf.l_len = len;

	if (fcntl(fd, F_GETLK, &flockbuf) < 0)
		err_sys("fcntl error");
	if (flockbuf.l_type == F_UNLCK)
		return 0;
	return flockbuf.l_pid;
}


/**
 * 设置一个打开文件描述符中的一个或多个描述符标志
 */
int set_fd(int fd, int flags) {
	int val;

	if ((val = fcntl(fd, F_GETFD, 0)) < 0)
		return -1;
	val |= flags;
	return fcntl(fd, F_SETFD, val);
}


/**
 * 清除一个打开文件描述符中的一个或多个描述符标志
 */
int clr_fd(int fd, int flags) {
	int val;

	if ((val = fcntl(fd, F_GETFD, 0)) < 0)
		return -1;
	val &= ~flags;
	return fcntl(fd, F_SETFD, val);
}


/**
 * 设置一个文件描述符指向的文件表项中的一个或多个文件状态标志
 */
int set_fl(int fd, int flags) {
	int val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		return -1;
	val |= flags;
	return fcntl(fd, F_SETFL, val);
}


/**
 * 清除一个文件描述符指向的文件表项中的一个或多个文件状态标志
 */
int clr_fl(int fd, int flags) {
	int val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		return -1;
	val &= ~flags;
	return fcntl(fd, F_SETFL, val);
}


/**
 * 从fd指定的文件中读取指定字节数的到指定缓冲区
 */
ssize_t readn(int fd, void* buf, size_t nbytes) {
	size_t nleft;
	size_t nread;

	nleft = nbytes;
	while (nleft > 0) {
		if ((nread = read(fd, buf, nleft)) < 0) {
			if (nleft == nbytes)
				return -1;
			break;
		}
		else if (nread == 0)
			break;
		nleft -= nread;
		buf += nread;
	}
	return nbytes - nleft;
}


/**
 * 写入指定缓冲区中的指定字节数到fd指向的文件中
 */
ssize_t writen(int fd, const void* buf, size_t nbytes) {
	size_t nleft;
	size_t nwrite;

	nleft = nbytes;
	while (nleft > 0) {
		if ((nwrite = write(fd, buf, nleft)) < 0) {
			if (nleft == nbytes)
				return -1;
			break;
		}
		else if (nleft == 0)
			break;
		nleft -= nwrite;
		buf += nwrite;
	}
	return nbytes - nleft;
}
