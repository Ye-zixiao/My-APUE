#include "../include/MyAPUE.h"
#include <string.h>

#define MAXLINE BUFSIZE

#if defined(SCM_CREDS)  //BSD interface
#define CREDSTRUCT cmsgcred
#define SCM_CREDTYPE SCM_CREDS
#elif defined(SCM_CREDENTIALS)  //Linux interface
#define CREDSTRUCT ucred
#define SCM_CREDTYPE SCM_CREDENTIALS
#else
#error passing credentials is unsupported!
#endif


/* 指明传送一个通过sendmsg()传送control message所需要的
 *    struct cmsghdr + 数据对象所需要的字节大小 */
#define RIGHTSLEN CMSG_LEN(sizeof(int))
#define CREDSLEN CMSG_LEN(sizeof(struct CREDSTRUCT))
#define CONTROLLEN (RIGHTSLEN+CREDSLEN)

static struct cmsghdr* cmptr = NULL;


/**
 * 通过UNIX域套接字连接的进程发送：
 *    '\0'(即字节0) + 1字节status(状态信息，正常时为0) + 控制消息(文件描述符+证书)
 */
int send_fd(int sockfd, int fd_to_send) {
    struct CREDSTRUCT* credp;
    struct cmsghdr* cmp;
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
        /* 若所传递的文件描述符无效 */
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        buf[1] = -fd_to_send;
        if (buf[1] == 0)//防止fd_to_send为-256时导致char变量上溢出而变成0
            buf[1] = 1;
    }
    else {
        /* 若所传递的文件描述符有效，则将struct msghdr中的
            控制消息成员(包括文件描述符 + 证书)部分进行设置 */
        if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
            return -1;
        msg.msg_control = cmptr;
        msg.msg_controllen = CONTROLLEN;

        cmp = cmptr;
        cmp->cmsg_len = RIGHTSLEN;
        cmp->cmsg_type = SOL_SOCKET;
        cmp->cmsg_type = SCM_RIGHTS;
        *(int*)CMSG_DATA(cmp) = fd_to_send;
        cmp = CMSG_NXTHDR(&msg, cmp);
        cmp->cmsg_len = CREDSLEN;
        cmp->cmsg_level = SOL_SOCKET;
        cmp->cmsg_type = SCM_CREDTYPE;
        credp = (struct CREDSTRUCT*)CMSG_DATA(cmp);

#ifdef SCM_CREDENTIALS  //初始化证书
        credp->uid = geteuid();
        credp->gid = getegid();
        credp->pid = getpid();
#endif

        buf[1] = 0;
    }

    buf[0] = '\0';
    if (sendmsg(sockfd, &msg, 0) != 2)
        err_sys("sendmsg error");
    return 0;
}


/**
 * 向通过UNIX域套接字连接的进程发送：
 *     errmsg(包含'\0') + 字节0 + errcode(即status)
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

/**
 * 从服务进程中接收：
 *     首部消息 + status + 文件描述符 或者 首部出错消息 + status + 出错fd等信息
 * 并将首部（出错）消息通过userfunc进行处理
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
        errmsg(不包含'\0') + char buf[2]（其中第一个是作为字符串的null结尾）+ errcode */
        for (ptr = buf; ptr < &buf[nrecv];) {
            if (*ptr++ == '\0') {           //定位到buf[0]
                if (ptr != &buf[nrecv - 1]) //定位到buf[1]
                    err_dump("message format error");
                status = *ptr & 0xFF;
                /* 若buf[1]这个表示状态的字节==0，则表示后续的是文件描述符 */
                if (status == 0) {
                    if (msg.msg_controllen < CONTROLLEN)
                        err_dump("status = 0 buf no fd");
                    newfd = *(int*)CMSG_DATA(cmptr);
                }
                else {
                    /* status不为0，则表示没有传送过来文件描述符，因为文件描述符在发送端
                        就已经被检测出来是无效的，所以此时的newfd会变成负数，指出这种错
                        误的情况*/
                    newfd = -status;
                }
                nrecv -= 2;
            }
        }
        //调用userfunc处理出错消息
        if (nrecv > 0 && (*userfunc)(STDERR_FILENO, buf, nrecv) != nrecv)
            return -1;
        if (status >= 0)
            return newfd;
    }
}
