#include "../include/MyAPUE.h"
#include <string.h>

#define CONTROLLEN(N) \
		CMSG_LEN(sizeof(int) * (N))


/**
 * 发送多个文件描述符到UNIX套接字描述符
 */
int send_fds(int sockfd, int fds_to_send[], int N) {
	struct cmsghdr* cmp;
	struct msghdr msg;
	struct iovec iov[1];
	char buf[32];
	int* pfd;

	sprintf(buf, "%d", N);
	if ((cmp = malloc(CONTROLLEN(N))) == NULL)
		return -1;
	msg.msg_control = cmp;
	msg.msg_controllen = CONTROLLEN(N);
	iov[0].iov_base = buf;
	iov[0].iov_len = strlen(buf) + 1;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_flags = 0;

	/* Linux中一个msghdr结构发送多个文件描述符，可以通过在唯一的cmsghdr
           结构体对象中分配足够的空间，然后在其中放入想发送的多个描述符。
	   但是我们不能通过一个msghdr结构体中打包进多个cmsghdr结构对象（每个
	   对象各自存储一个文件描述符）的方式来达到发送多个文件描述符的目的
	   （好像是只有FreeBSD支持的缘故）*/
	cmp->cmsg_len = CONTROLLEN(N);
	cmp->cmsg_level = SOL_SOCKET;
	cmp->cmsg_type = SCM_RIGHTS;
	pfd = (int*)CMSG_DATA(cmp);
	for (int i = 0; i < N; i++) {
		*pfd++ = fds_to_send[i];
	}

	if (sendmsg(sockfd, &msg, 0) < 0) {
		debug();
		free(cmp);
		return -2;
	}
	free(cmp);
	return 0;
}


/**
 * 从UNIX套接字描述符中接收多个文件描述符
 */
int recv_fds(int sockfd, int fds[], int N) {
	struct cmsghdr* cmp;
	struct msghdr msg;
	struct iovec iov[1];
	int num, nrcv, * pfd;
	char buf[32];

	memset(fds, -1, sizeof(int) * N);
	if ((cmp = malloc(CONTROLLEN(N))) == NULL)
		return -1;
	msg.msg_control = cmp;
	msg.msg_controllen = CONTROLLEN(N);
	iov[0].iov_base = buf;
	iov[0].iov_len = 32;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_flags = 0;
	if ((nrcv = recvmsg(sockfd, &msg, 0)) < 0) {
		free(cmp);
		err_ret("recvmsg error");
		return -2;
	}
	else if (nrcv == 0) {
		free(cmp);
		err_ret("recvmsg error");
		return -3;
	}

	/* 这里是通过sendmsg中传送过来的字符串指出文件描述符的个数 */
	if ((num = atoi(buf)) > N) {
		free(cmp);
		return -4;
	}
	pfd = (int*)CMSG_DATA(cmp);
	for (int i = 0; i < num; i++)
		fds[i] = *pfd++;

	free(cmp);
	return num;
}


int main(int argc, char* argv[])
{
	int fds[10], pairfd[2];
	pid_t pid;
	int ret;

	if (argc < 2)
		err_quit("usage: %s <filename...>", argv[0]);
	if (fd_pipe(pairfd) == -1)
		err_sys("fd_pipe error");

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		close(fds[0]);
		for (int i = 1; i < argc; ++i) {
			if ((fds[i - 1] = open(argv[i], O_RDONLY)) < 0)
				err_sys("open error");
		}
		if (send_fds(pairfd[1], fds, argc - 1) < 0)
			err_sys("send_fds error");
		printf("child : send %d fds to parent\n", argc - 1);
	}
	else {
		close(pairfd[1]);
		if ((ret = recv_fds(pairfd[0], fds, 10)) < 0)
			err_sys("recv_fds error");
		printf("parent: received %d fds from child, includes:\n", ret);
		for (int i = 0; i < ret; i++)
			if (fds[i] >= 0)
				printf("\tfd %d", fds[i]);
		putchar('\n');
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
	}

	exit(EXIT_SUCCESS);
}
