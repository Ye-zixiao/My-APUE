#include "../include/MyAPUE.h"
#include <sys/un.h>


/**
 * 创建一个服务器连接端点，完成socket---->bind----->listen流程，宣告
 * 在一个与指定文件路径（一个众所周知的名字）绑定的套接字上监听客户进
 * 程的连接请求，并在调用成功后返回与之关联的UNIX套接字描述符
 */
int server_listen(const char* name, int qlen) {
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
	//umask(S_IRWXG | S_IRWXO);//限制创建socket文件的使用权限
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


#define QLEN 10

int serv_listen(const char* name) {
	return server_listen(name, QLEN);
}


//不允许客户进程关联的socket文件旧于该STALE秒之前
#define STALE 30

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
	char* name;

	alen = sizeof(unaddr);
	if ((name = malloc(sizeof(unaddr.sun_path) + 1)) == NULL)
		return -1;
	if ((clientfd = accept(listenfd, (struct sockaddr*)&unaddr, &alen)) == -1) {
		free(name);
		return -2;
	}

	/* 获取socket文件元数据信息，检查文件类型、文件权限以及
		文件时间是否存在问题 */
	alen -= offsetof(struct sockaddr_un, sun_path);
	memcpy(name, unaddr.sun_path, alen);
	name[alen] = '\0';
	if (stat(name, &statbuf) == -1) {
		rval = -3;
		goto errout;
	}

#ifdef S_ISSOCK
	if (S_ISSOCK(statbuf.st_mode) == 0) {
		rval = -4;
		goto errout;
	}
#endif
	if ((statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
		(statbuf.st_mode & S_IRWXU) != S_IRWXU) {
		rval = -5;
		goto errout;
	}

	staletime = time(NULL) - STALE;
	if (statbuf.st_atim.tv_sec < staletime || statbuf.st_ctim.tv_sec < staletime ||
		statbuf.st_mtim.tv_sec < staletime) {
		rval = -6;
		goto errout;
	}

	if (uidptr != NULL)
		*uidptr = statbuf.st_uid;
	//服务进程在与客户进程连接成功后删除客户进程因bind创建的socket文件
	unlink(name);
	free(name);
	return clientfd;

errout:
	err = errno;
	close(clientfd);
	free(name);
	errno = err;
	return rval;
}


#define CLI_PATH "/var/tmp/"
#define CLI_PERM S_IRWXU
/**
 * 创建一个客户进程通信端点，并尝试与服务进程进行连接
 */
int cli_conn(const char* name) {
	struct sockaddr_un unaddr, svunaddr;
	int clifd, err, rval;
	socklen_t alen;
	int do_unlink;

	do_unlink = 0;
	if (strlen(name) >= sizeof(unaddr.sun_path)) {
		errno = ENAMETOOLONG;
		return -1;
	}
	if ((clifd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return -1;

	/* 将客户进程的通信端点UNIX域套接字与一个进程相关的地址进行绑定，
		这样可以使得服务进程可以很好的区分客户进程 */
	memset(&unaddr, 0, sizeof(unaddr));
	unaddr.sun_family = AF_UNIX;
	sprintf(unaddr.sun_path, "%s%05d", CLI_PATH, getpid());
	alen = offsetof(struct sockaddr_un, sun_path) + strlen(unaddr.sun_path);
	unlink(unaddr.sun_path);
	if (bind(clifd, (struct sockaddr*)&unaddr, alen) == -1) {
		rval = -2;
		goto errout;
	}
	if (chmod(unaddr.sun_path, CLI_PERM) == -1) {
		rval = -3;
		do_unlink = 1;
		goto errout;
	}

	/* 与服务进程进行连接 */
	memset(&svunaddr, 0, sizeof(svunaddr));
	svunaddr.sun_family = AF_UNIX;
	strcpy(svunaddr.sun_path, name);
	alen = offsetof(struct sockaddr_un, sun_path) + strlen(svunaddr.sun_path);
	if (connect(clifd, (struct sockaddr*)&svunaddr, alen) == -1) {
		rval = -4;
		do_unlink = 1;
		goto errout;
	}
	return clifd;

errout:
	err = errno;
	close(clifd);
	if (do_unlink)
		unlink(unaddr.sun_path);
	errno = err;
	return rval;
}
