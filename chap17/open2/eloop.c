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
 * 
 */
void eloop(void) {
	int listenfd, clifd, nread, epollfd;
	int numfd = 1, maxfd = NALLOC, rfds;
	struct epoll_event ev, * pev;
	char buf[MAXLINE];
	uid_t uid;

	if ((pev = malloc(sizeof(struct epoll_event) * NALLOC)) == NULL)
		log_sys("malloc error");
	if ((epollfd = epoll_create(NALLOC)) == -1)
		log_sys("epoll_create error");

	if ((listenfd = serv_listen(CS_OPEN)) < 0)
		log_sys("serv_listen errror");
	client_add(listenfd, 0);

	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
		log_sys("epoll_ctl error");

	for (;;) {
		if ((rfds = epoll_wait(epollfd, pev, numfd, -1)) == -1)
			log_sys("epoll_wait error");

		for (int i = 0; i < rfds; ++i) {
			if (pev[i].data.fd == listenfd) {
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
			else if (pev[i].events & EPOLLHUP)
				goto hungup;
			else {
				int index = client_index(pev[i].data.fd);

				if ((nread = read(pev[i].data.fd, buf, MAXLINE)) < 0)
					log_sys("read error on fd %d", pev[i].data.fd);
				else if (nread == 0) {
hungup:
					log_msg("closed: uid %d, fd %d", client[index].uid, client[index].fd);
					client_del(pev[i].data.fd);
					close(pev[i].data.fd);
					numfd--;
				}
				else {
					handle_request(buf, nread, pev[i].data.fd, client[index].uid);
				}
			}
		}
	}

}
