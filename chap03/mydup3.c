/*
 * 练习题3-2
 * */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define BUFSIZE 4096
#define DEBUG

int mydup2(int oldfd, int newfd) {
	int val;
	if (oldfd != newfd) {
		close(newfd);
		while ((val = dup(oldfd)) < newfd)//缺点就是会大量占用oldfd与newfd之间的文件描述符值
			if (val < 0)return val;
	}
	return newfd;
}

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(-1);
}


int main(void) {
	int newfd, val;
	char buf[BUFSIZE];

	if ((newfd = mydup2(STDOUT_FILENO, 5)) < 0)
		err_sys("dup2 error\n");
#ifdef DEBUG
	printf("newfd set success!newfd=%d\n", newfd);
#endif

	while ((val = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
		if (write(newfd, buf, val) != val)
			err_sys("write error\n");
	if (val < 0)err_sys("write error\n");

	exit(0);
}
