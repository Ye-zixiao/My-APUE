#include "../include/MyAPUE.h"
#include <fcntl.h>


/**
 * 使用fcntl函数设置文件记录锁.
 * @param fd 文件描述符
 * @return 是否设置成功
 */
int lockfile(int fd) {
	struct flock flo;

	flo.l_type = F_WRLCK;
	flo.l_start = 0;
	flo.l_whence = SEEK_SET;
	flo.l_len = 0;
	return fcntl(fd, F_SETLK, &flo);
}


/**
 * 设置一个打开文件描述符中的一个或多个描述符标志.
 * @param fd 文件描述符
 * @param flags 欲加入的文件描述符标志
 * @return 是否成功
 */
int set_fd(int fd, int flags) {
	int val;

	if ((val = fcntl(fd, F_GETFD, 0)) < 0)
		return -1;
	val |= flags;
	return fcntl(fd, F_SETFD, val);
}


/**
 * 清除一个打开文件描述符中的一个或多个描述符标志. 
 * @param fd 文件描述符
 * @param flags 欲清理的文件描述符标志
 * @return 是否成功
 */
int clr_fd(int fd, int flags) {
	int val;

	if ((val = fcntl(fd, F_GETFD, 0)) < 0)
		return -1;
	val &= ~flags;
	return fcntl(fd, F_SETFD, val);
}


/**
 * 设置一个文件描述符指向的文件表项中的一个或多个文件状态标志.
 * @param fd 文件描述符
 * @param flags 欲加入的文件状态标志
 * @return 是否成功
 */
int set_fl(int fd, int flags) {
	int val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		return -1;
	val |= flags;
	return fcntl(fd, F_SETFL, val);
}


/**
 * 清除一个文件描述符指向的文件表项中的一个或多个文件状态标志.
 * @param fd 文件描述符
 * @param flags 欲清除的文件状态标志
 * @return 是否成功
 */
int clr_fl(int fd, int flags) {
	int val;
	
	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		return -1;
	val &= ~flags;
	return fcntl(fd, F_SETFL, val);
}
