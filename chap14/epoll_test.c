//#define DEBUG
#include "../include/MyAPUE.h"
#include <sys/epoll.h>

int main(void)
{
	char buf[BUFSIZE];
	int ofd, val, epfd;
	struct epoll_event ev;

	//1、初始化fds
	ofd = dup(STDOUT_FILENO);
	if ((val = read(STDIN_FILENO, buf, BUFSIZE)) < 0)
		err_sys("read error");
	
	//2、创建epoll instance实例，并向epoll对象中添加fds
	ev.data.fd = ofd;
	ev.events = EPOLLOUT;
	if ((epfd = epoll_create1(0)) < 0)
		err_sys("epoll_create1 error");
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, ofd, &ev) < 0)
		err_sys("epoll_ctl error");

	//3、使用epoll_wait阻塞线程等待，直到有文件描述符变得可用
	if (epoll_wait(epfd, &ev, 1, -1) > 0) {
#ifdef DEBUG
		printf("val = %d\n", val);
#endif
		write(ev.data.fd, buf, val);
	}

	exit(EXIT_SUCCESS);
}
