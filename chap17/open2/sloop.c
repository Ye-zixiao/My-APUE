#include "opend.h"
#include <sys/select.h>


/**
 * 使用select监听感兴趣的套接字描述符，在任一套接字描述符可用的
 * 时候分配一个新的套接字（接受一个客户进程的连接请求）或者从套
 * 接字描述符中读取客户进程关于打开文件的请求，服务进程会相应的
 * 做出响应，或者做出错误提示、关闭连接等操作
 */
void sloop(void) {
	int n, index, maxfd, maxindex, listenfd, clifd, nread;
	fd_set rset, allset;
	char buf[MAXLINE];
	uid_t uid;

	FD_ZERO(&allset);

	if ((listenfd = serv_listen(CS_OPEN)) < 0)
		log_sys("serv_listen error");
	FD_SET(listenfd, &allset);
	maxfd = listenfd;
	maxindex = -1;


	for (;;) {
		rset = allset;
		if ((n = select(maxfd + 1, &rset, NULL, NULL, NULL)) == -1)
			log_sys("select error");

		/* 若收到客户进程的连接请求，则接受连接并新建一个套接字与之连接，
			然后将其添加到select的监听列表之中 */
		if (FD_ISSET(listenfd, &rset)) {
			if ((clifd = serv_accept(listenfd, &uid)) < 0)
				log_sys("serv_accept error: %d", clifd);
			index = client_add(clifd, uid);
			FD_SET(clifd, &allset);
			if (clifd > maxfd)
				maxfd = clifd;
			if (index > maxindex)
				maxindex = index;
			log_msg("new connection: uid %d, fd %d", uid, clifd);
			continue;
		}

		/* 处理从客户进程发送过来的文件打开请求然后向其发送文件描述符，或
			处理客户与服务进程的关闭事件 */
		for (int i = 0; i <= maxindex; ++i) {
			if ((clifd = client[i].fd) < 0)
				continue;
			if (FD_ISSET(clifd, &allset)) {
				if ((nread = read(clifd, buf, MAXLINE)) < 0)
					log_sys("read error on fd %d", clifd);
				else if (nread == 0) {
					log_msg("closed: uid %d, fd %d", client[index].uid, client[index].fd);
					client_del(clifd);
					FD_CLR(clifd, &allset);
					close(clifd);
				}
				else {
					handle_request(buf, nread, clifd, uid);
				}
			}
		}
	}
}