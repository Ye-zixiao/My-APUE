#include "open.h"
#include <string.h>
#include <sys/uio.h>


/**
 * 向服务进程发送打开指定文件请求，服务进程则对应地打开相应
 * 的文件，并传送回对应的文件描述符
 */
int csopen(char* pathname, int oflag) {
	static int csfd = -1;
	struct iovec iov[3];
	char buf[12];
	size_t len;

	if (csfd < 0) {
		if ((csfd = cli_conn(CS_OPEN)) < 0) {
			err_ret("cli_conn error");
			return -1;
		}
	}

	//其实这个oflag根本就不能用户自定义！💩
	sprintf(buf, " %d", oflag);
	iov[0].iov_base = CL_OPEN" ";
	iov[0].iov_len = strlen(CL_OPEN) + 1;
	iov[1].iov_base = pathname;
	iov[1].iov_len = strlen(pathname);
	iov[2].iov_base = buf;
	iov[2].iov_len = strlen(buf) + 1;
	len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len;
	if (writev(csfd, iov, 3) != len) {
		err_ret("writev error");
		return -1;
	}
	return recv_fd(csfd, write);
}