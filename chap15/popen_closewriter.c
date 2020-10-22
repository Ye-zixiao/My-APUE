#include "../include/MyAPUE.h"

static void sig_pipe(int signo) {
	fprintf(stderr, "caught SIGPIPE\n");
	exit(EXIT_FAILURE);
}

int main(void)
{
	FILE* fp;

	if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
		err_sys("signal error");
	/**
	 * 若popen是以r模式打开的，则popen中执行的cmdstring进程终止了会导致
	 * 管道中的读者进程收到文件接受标志。
	 * 但是若popen是以w模式打开的，则popen中执行的cmdstring进程终止后会
	 * 导致管道读文件描述符被关闭，此时写者进程继续向管道中写入数据会接收
	 * 到SIGPIPE信号
	 */
	if ((fp = Popen("date", "w")) == NULL)
		err_sys("popen error");
	sleep(1);
	if (fputs("hello world\n", fp) == EOF)
		err_sys("fputs error");

	exit(EXIT_SUCCESS);
}
