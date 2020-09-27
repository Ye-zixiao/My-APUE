#include "../include/MyAPUE.h"

/**
 * 该指针指向元素类型为pid_t的数组，其中数组下标记录了父进程打开了哪
 * 些文件描述符关联到管道，元素中的内容记录了这个管道连接到哪个子进程
 * (子进程的进程号在调用Pclose的时候可以用于waitpid)。
 * 注意：这个数组只对于父进程而言是有用的
 */
static pid_t* childpid = NULL;

static int maxfd;


/**
 * 打开管道，并创建子进程，使用管道连接父进程与子进程，
 * 并返回管道写端/读端文件指针
 */
FILE* Popen(const char* cmdstring, const char* type) {
	int pfds[2];
	pid_t pid;
	FILE* fp;

	if ((type[0] != 'r' && type[0] != 'w') || type[1] != '\0') {
		errno = EINVAL;
		return NULL;
	}
	if (childpid == NULL) {
		maxfd = open_max();
		if ((childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
			return NULL;
	}
	if (pipe(pfds) < 0)
		return NULL;
	if (pfds[0] >= maxfd || pfds[1] >= maxfd) {
		close(pfds[0]);
		close(pfds[1]);
		errno = EMFILE;
		return NULL;
	}


	if ((pid = fork()) < 0) {
		close(pfds[0]);
		close(pfds[1]);
		return NULL;
	}
	else if (pid == 0) {
		//子进程重新设置stdin、stdout并关闭不使用的文件描述符
		if (type[0] == 'r') {
			close(pfds[0]);
			if (pfds[1] != STDOUT_FILENO) {
				dup2(pfds[1], STDOUT_FILENO);
				close(pfds[1]);
			}
		}
		else if (type[0] == 'w') {
			close(pfds[1]);
			if (pfds[0] != STDIN_FILENO) {
				dup2(pfds[0], STDIN_FILENO);
				close(pfds[0]);
			}
		}

		/**
		 * 关闭childpid数组中记录的有效文件描述符。子进程不会使用到它们，
		 * 为了避免在子进程中这些文件描述符被不良的占用，子进程应该将它们
		 * 关闭
		 */
		for (int i = 0; i < maxfd; ++i)
			if (childpid[i] > 0)
				close(i);

		execl("/bin/sh", "sh", "-c", cmdstring, (char*)NULL);
		//这里执行失败不会导致Popen返回NULL
		_exit(127);
	}

	/**
	 * 以文件形式打开管道文件描述符，并返回对应文件指针
	 */
	if (type[0] == 'r') {
		close(pfds[1]);
		if ((fp = fdopen(pfds[0], "r")) == NULL)
			return NULL;
	}
	else {
		close(pfds[0]);
		if ((fp = fdopen(pfds[1], "w")) == NULL)
			return NULL;
	}

	childpid[fileno(fp)] = pid;
	return fp;
}


/**
 * 关闭子进程与父进程之间的管道，也意味着会关闭父进程获得的管道文件
 * 指针，并等待子进程的终止，并获取其终止状态
 */
int Pclose(FILE* fp) {
	int fd, stat;
	pid_t pid;

	//检查这个文件指针封装的文件描述符是否真的与某一个管道关联
	if (childpid == NULL) {
		errno = EINVAL;
		return -1;
	}
	fd = fileno(fp);
	if (fd >= maxfd) {
		errno = EINVAL;
		return -1;
	}
	if ((pid = childpid[fd]) == 0) {
		errno = EINVAL;
		return -1;
	}

	//重置childpid
	childpid[fd] = 0;
	if (fclose(fp) < 0)
		return -1;

	/**
	 * 若waitpid是因信号中断而失败返回，则Pclose失败返回，
	 * 而errno会被waitpid自动设置为ECHILD
	 */
	while (waitpid(pid, &stat, 0) < 0)
		if (errno != EINTR)
			return -1;
	return stat;
}
