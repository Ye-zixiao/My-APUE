#include "opend.h"
#include <poll.h>

#define NALLOC 10


/**
 * 分配或者重新分配pollfd结构体数组，使其能够监听更多的感兴趣
 * （套接字）文件描述符
 */
static struct pollfd* grow_pollfd(struct pollfd* pfd, int* maxfd) {
	int oldmax = *maxfd;
	int newmax = oldmax + NALLOC;

	if ((pfd = realloc(pfd, newmax * sizeof(struct pollfd))) == NULL)
		err_sys("realloc error");
	for (int i = oldmax; i < newmax; ++i) {
		pfd[i].fd = -1;
		pfd[i].fd = POLLIN;
		pfd[i].revents = 0;
	}
	*maxfd = newmax;
	return pfd;
}


/**
 * 使用poll监听感兴趣的套接字描述符，在任一套接字描述符可用的
 * 时候分配一个新的套接字（接受一个客户进程的连接请求）或者从套
 * 接字描述符中读取客户进程关于打开文件的请求，服务进程会相应的
 * 做出响应，或者做出错误提示、关闭连接等操作
 */
void ploop(void) {
	int maxfd = NALLOC, numfd = 1, index;
	int nread, listenfd, clifd, fd;
	struct pollfd* pfd;
	char buf[MAXLINE];
	uid_t uid;

	if ((pfd = malloc(sizeof(struct pollfd) * NALLOC)) == NULL)
		err_sys("malloc error");
	for (int i = 0; i < NALLOC; ++i) {
		pfd[i].fd = -1;
		pfd[i].events = POLLIN;
		pfd[i].revents = 0;
	}

	/* 服务进程创建监听套接字 */
	if ((listenfd = serv_listen(CS_OPEN)) < 0)
		log_sys("serv_listen error");
	client_add(listenfd, 0);//uid==0表示listenfd
	pfd[0].fd = listenfd;


	for (;;) {
		if (poll(pfd, numfd, -1) == -1)
			log_sys("poll error");

		/* 若listenfd描述符变得有效，这表明接收到客户进程的连接建立请求 */
		if (pfd[0].revents & POLLIN) {
			if ((clifd = serv_accept(listenfd, &uid)) < 0)
				log_sys("serv_accept error");
			client_add(clifd, uid);

			if (numfd == maxfd)
				pfd = grow_pollfd(pfd, &maxfd);
			pfd[numfd].fd = clifd;
			pfd[numfd].events = POLLIN;
			pfd[numfd].revents = 0;
			numfd++;
			log_msg("new connection: uid %d, fd %d", uid, clifd);
		}

		/* 遍历所有的pollfd结构体数组成员，以检查是那些套接字描述符变得有效，
			并做出相应的请求处理或者当连接关闭的时候做出善后处理 */
		for (int i = 1; i < numfd; i++) {
			fd = pfd[i].fd;
			index = client_index(fd);

			if (pfd[i].revents & POLLHUP)
				goto hungup;
			else if (pfd[i].revents & POLLIN) {
				if ((nread = read(fd, buf, MAXLINE)) < 0)
					log_sys("read error on fd %d", fd);
				else if (nread == 0) {
hungup:
					log_msg("closed: uid %d, fd %d", client[index].uid, fd);
					client_del(fd);
					close(fd);
					if (i < (numfd - 1)) {
						pfd[i].fd = pfd[numfd - 1].fd;
						pfd[i].events = pfd[numfd - 1].events;
						pfd[i].revents = pfd[numfd - 1].revents;
						i--;
					}
					numfd--;
				}
				else {
					handle_request(buf, nread, fd, client[index].uid);
				}
			}
		}
	}
}
