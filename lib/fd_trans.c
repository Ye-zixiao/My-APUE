#include "MyAPUE.h"
#include <string.h>

#define MAXLINE BUFSIZE
#define CONTROLLEN CMSG_LEN(sizeof(int))

static struct cmsghdr* cmptr = NULL;


/**
 * 1、正常时向UNIX域套接字连接的客户进程发送：
 *    '\0'(即字节0) + 字节0(status) + 控制信息(文件描述符+证书)
 *
 * 2、异常时向UNIX域套接字连接的客户进程发送(此时该函数是专门给send_err使用的)
 *    '\0'(即字节0) + 一个负数的绝对值字节
 */
int send_fd(int sockfd, int fd_to_send) {
	struct iovec iov[1];
	struct msghdr msg;
	char buf[2];

	iov[0].iov_base = buf;
	iov[0].iov_len = 2;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	if (fd_to_send < 0) {
		/* 若欲传送文件描述符无效 */
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		buf[1] = -fd_to_send;
		if (buf[1] == 0)//-256防止上溢出
			buf[1] = 1;
	}
	else {
		/* 若欲传送文件描述符有效 */
		if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
			return -1;
		msg.msg_control = cmptr;
		msg.msg_controllen = CONTROLLEN;

		cmptr->cmsg_len = CONTROLLEN;
		cmptr->cmsg_level = SOL_SOCKET;
		cmptr->cmsg_type = SCM_RIGHTS;
		*(int*)CMSG_DATA(cmptr) = fd_to_send;
		buf[1] = '\0';
	}
	
	buf[0] = '\0';
    if (sendmsg(sockfd, &msg, 0) != 2) {
        return -1;
    }
	return 0;
}


/**
 * 从服务进程中接收控制信息，其中前面的出错消息传递给userfunc
 * 进行处理
 */
int recv_fd(int sockfd, ssize_t(*userfunc)(int, const void*, size_t)) {
    int newfd, nrecv, status;
    struct iovec iov[1];
    struct msghdr msg;
    char buf[MAXLINE];
    char* ptr;

    status = -1;
    for (;;) {
        /* 初始化供recvmsg()函数使用的msghdr结构体 */
        iov[0].iov_base = buf;
        iov[0].iov_len = sizeof(buf);
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
            return -1;
        msg.msg_control = cmptr;
        msg.msg_controllen = CONTROLLEN;
        if ((nrecv = recvmsg(sockfd, &msg, 0)) < 0) {
            err_ret("recvmsg error");
            return -1;
        }
        else if (nrecv == 0) {
            err_ret("connection closed by server");
            return -1;
        }

        /*   遍历send_fd()发送过来的char buf[2] + fd，或者send_err()发送过来的
        errmsg(不包含'\0') + char buf[2]（buf[0]作为字符串的null结尾）+ errcode */
        for (ptr = buf; ptr < &buf[nrecv];) {
            if (*ptr++ == '\0') {           //定位到buf[0]
                if (ptr != &buf[nrecv - 1]) //定位到buf[1]
                    err_dump("message format error");
                status = *ptr & 0xFF;
                /* 若buf[1]这个表示状态的字节==0，则表示后续的是文件描述符，
                    否则表示没有服务器进程没有发送过来文件描述符，此时需要将
                    该status转换回errcode，然后返回 */
                if (status == 0) {
                    if (msg.msg_controllen < CONTROLLEN)
                        err_dump("status = 0 buf no fd");
                    newfd = *(int*)CMSG_DATA(cmptr);
                }
                else {
                    newfd = -status;
                }
                nrecv -= 2;
            }
        }

        if (nrecv > 0 && (*userfunc)(STDERR_FILENO, buf, nrecv) != nrecv)
            return -1;
        if (status >= 0)
            return newfd;
    }
}


/**
 * 向通过UNIX域套接字连接的客户进程发送：
 *     errmsg(不包含'\0') + '\0'(即字节0) + errcode(即status)
 */
int send_err(int sockfd, int errcode, const char* errmsg) {
    int n;

    if ((n = strlen(errmsg)) > 0)
        if (writen(sockfd, errmsg, n) != n)
            return -1;
    if (send_fd(sockfd, errcode < 0 ? errcode : -1) == -1)
        return -1;
    return 0;
}
