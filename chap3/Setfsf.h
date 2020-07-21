/*
 *用于设置文件的状态标识符
 * */
#include"apue.h"
#include<fcntl.h>

void set_fsflag(int fd, int flags) {
	int val;
	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");
	
	val |= flags;

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}

void close_fsflag(int fd, int flags) {
	int val;
	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val &= ~flags;

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}
