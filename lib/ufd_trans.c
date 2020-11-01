/**
 * 注意：在一个msghdr中打包进多个cmsghdr结构体对象，然后试图这种方式
 * 发送多个控制消息（例如多个文件描述符）可能在Linux中得不到支持，好像
 * 只有FreeBSD支持！
 */
#include "../include/MyAPUE.h"
#include <string.h>

#define MAXLINE BUFSIZE
#define RIGHTSLEN CMSG_LEN(sizeof(int))
#define CREDSLEN	 CMSG_LEN(sizeof(struct ucred))
#define CONTROLLEN (RIGHTSLEN + CREDSLEN)

static struct cmsghdr* cmptr = NULL;


/**
 * 在send_fd()函数的基础上增加了发送用户证书的能力
 */
int send_ufd(int sockfd, int fd_to_send) {
	struct ucred* credp;
	struct cmsghdr* cmp;
	struct iovec iov[1];
	struct msghdr msg;
	char buf[2];

	iov[0].iov_base = buf;
	iov[0].iov_len = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;
	
	if (fd_to_send < 0) {
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		buf[1] = -fd_to_send;
		if (buf[1] == 0)
			buf[1] = 1;
	}
	else {
		if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
			return -1;
		msg.msg_control = cmptr;
		msg.msg_controllen = CONTROLLEN;

		cmp = CMSG_FIRSTHDR(&msg);
		cmp->cmsg_len = RIGHTSLEN;
		cmp->cmsg_level = SOL_SOCKET;
		cmp->cmsg_type = SCM_RIGHTS;
        *(int*)CMSG_DATA(cmp) = fd_to_send;

		cmp = CMSG_NXTHDR(&msg, cmp);
		cmp->cmsg_len = CREDSLEN;
		cmp->cmsg_level = SOL_SOCKET;
		cmp->cmsg_type = SCM_CREDENTIALS;
		credp = (struct ucred*)CMSG_DATA(cmp);
		credp->uid = getuid();
		credp->pid = getpid();
		credp->gid = getgid();

		buf[1] = 0;
	}

	buf[0] = 0;
	if (sendmsg(sockfd, &msg, 0) != 2)
		return -1;
	return 0;
}


/**
 * 从服务进程中接收控制信息，该函数recv_ufd不仅能够接收文件描述符，
 * 而且还能够接收用户证书以指出用户ID
 */
int recv_ufd(int sockfd, uid_t* uidptr,
    ssize_t(*userfunc)(int, const void*, size_t)) {
    struct ucred* credp;
    struct cmsghdr* cmp;
    struct iovec iov[1];
    struct msghdr msg;
    char buf[MAXLINE];
    int newfd = -1;
    int status = -1;
    char* ptr;
    int nrecv;

#ifdef SO_PASSCRED
    const int on = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(int)) == -1) {
        err_ret("setsockopt error");
        return -1;
    }
#endif

    for (;;) {
        /* 初始化recvmsg函数所需要的msghdr结构体 */
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

        for (ptr = buf; ptr < &buf[nrecv];) {
            if (*ptr++ == 0) {  //定位到buf[0]
                if (ptr != &buf[nrecv - 1]) //定位到buf[1]
                    err_dump("message format error");
                /* 检查buf[1]代表的状态字status是否正常，若为0，
                     则表示后续部分携带了控制消息 */
                status = *ptr & 0xFF;
                if (status == 0) {
                    if (msg.msg_controllen != CONTROLLEN)
                        err_dump("status == 0, but no fd");

                    /* 迭代获取struct msghdrmsg_control指向的struct cmsghdr连续数组
                        记录的控制消息（文件描述符+用户证书） */
                    for (cmp = CMSG_FIRSTHDR(&msg); cmp != NULL;
                        cmp = CMSG_NXTHDR(&msg, cmp)) {
                        if (cmp->cmsg_level != SOL_SOCKET)
                            continue;
                        switch (cmp->cmsg_type)
                        {
                        case SCM_RIGHTS:
                            newfd = *(int*)CMSG_DATA(cmp);
                            break;
                        case SCM_CREDENTIALS:
                            credp = (struct ucred*)CMSG_DATA(cmp);
                            *uidptr = credp->uid;
                        }
                    }
                }
                else {
                    newfd = -status;
                }
            }
            nrecv -= 2;
        }
        if (nrecv > 0 && (*userfunc)(STDERR_FILENO, buf, nrecv) != nrecv)
            return -1;
        if (status >= 0)
            return newfd;
    }
}
