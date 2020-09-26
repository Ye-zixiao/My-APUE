#include "../include/MyAPUE.h"
#include <string.h>

#define DEFAULT_PAGER "/usr/bin/more"

int main(int argc, char* argv[])
{
	pid_t pid;
	int fds[2];
	int val, ifd;
	char* pager, *argv0;
	char databuf[BUFSIZE];

	if (argc != 2)
		err_quit("usage: %s <filename>", argv[0]);
	if ((ifd = open(argv[1], O_RDONLY)) < 0)
		err_sys("open error");
	if (pipe(fds) < 0)
		err_sys("pipe error");
	
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid > 0) {
		close(fds[0]);

		//父进程将指定文件输入到管道之中
		while ((val = read(ifd, databuf, BUFSIZE)) > 0) {
			if (write(fds[1], databuf, val) != val)
				err_sys("write error");
		}
		if (val < 0)
			err_sys("read error");
		close(fds[1]);

		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
	}
	else {
		close(fds[1]);
		
		//子进程关闭原来的stdin，并使其指向fds[0]文件描述符原来指向的文件
		if (fds[0] != STDIN_FILENO) {
			if (dup2(fds[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error");
			close(fds[0]);
		}
		//检测是否有设置相关的环境变量，若设置了则优先使用环境变量指定的内容
		if ((pager = getenv("PAGER")) == NULL)
			pager = DEFAULT_PAGER;
		if ((argv0 = strrchr(pager, '/')) == NULL)
			argv0 = pager;
		else argv0++;
		if (execlp(pager, argv0, (char*)NULL) < 0)
			err_sys("execlp error");
	}

	exit(EXIT_SUCCESS);
}
