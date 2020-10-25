#include "opend.h"
#include <sys/epoll.h>

#define NALLOC 10


/**
 * 重新分配epoll_event结构体数组，使其能够监听更多的感兴趣
 * （套接字）文件描述符
 */
static struct epoll_event* 
grow_epoll(struct epoll_event* pev, int* maxfd) {
	int oldmax = *maxfd;
	int newmax = oldmax + NALLOC;

	if ((pev = realloc(pev, newmax * sizeof(struct epoll_event))) == NULL)
		log_sys("realloc error");
	*maxfd = newmax;
	return pev;
}


/**
 * 使用epoll监听感兴趣的套接字描述符，在任一套接字描述符可用的时候
 * 分配一个新的套接字（接受一个客户进程的连接请求）或者从套接字描述
 * 符中读取客户进程关于打开文件的请求，服务进程会相应的做出响应，或
 * 者做出错误提示、关闭连接等操作
 */
void eloop(void) {
	int nread, index, numfd = 1, maxfd = NALLOC;
	int epollfd, listenfd, clifd, rnfds, fd;
	struct epoll_event ev, * pev;
	char buf[MAXLINE];
	uid_t uid;

	if ((pev = malloc(sizeof(struct epoll_event) * NALLOC)) == NULL)
		log_sys("malloc error");
	//创建epoll对象
	if ((epollfd = epoll_create(NALLOC)) == -1)
		log_sys("epoll_create error");

	/* 服务进程创建监听套接字将其纳入到epoll的感兴趣列表中 */
	if ((listenfd = serv_listen(CS_OPEN)) < 0)
		log_sys("serv_listen errror");
	client_add(listenfd, 0);
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
		log_sys("epoll_ctl error");


	for (;;) {
		if ((rnfds = epoll_wait(epollfd, pev, numfd, -1)) == -1)
			log_sys("epoll_wait error");

		for (int i = 0; i < rnfds; ++i) {
			fd = pev[i].data.fd;
			index = client_index(fd);

			/* 若是listenfd描述符变得有效，则表明有新的客户连接请求 */
			if (fd == listenfd) {
				if ((clifd = serv_accept(listenfd, &uid)) < 0)
					log_sys("serv_accept error: %d", clifd);
				client_add(clifd, uid);

				if (numfd == maxfd)
					pev = grow_epoll(pev, &maxfd);
				ev.events = EPOLLIN;
				ev.data.fd = clifd;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clifd, &ev) == -1)
					log_sys("epoll_ctl error");
				numfd++;
				log_msg("new connect: uid %d, fd %d", uid, clifd);
			}
			/* 若是有描述符身上发生EPOLLHUP事件，这表明客户进程
				与服务进程连接断开 */
			else if (pev[i].events & EPOLLHUP) {
				goto hungup;
			}
			/* 若是非listenfd套接字描述符变得有效这可能是客户进程发送过来了
				打开文件请求，也有可能是发生了对方关闭连接事件 */
			else {
				if ((nread = read(fd, buf, MAXLINE)) < 0)
					log_sys("read error on fd %d", fd);
				else if (nread == 0) {
hungup:
					log_msg("closed: uid %d, fd %d", client[index].uid, fd);
					client_del(fd);
					close(fd);
					numfd--;
				}
				else {
					handle_request(buf, nread, fd, client[index].uid);
				}
			}
		}
	}

}
