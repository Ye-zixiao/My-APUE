#include"../include/MyAPUE.h"

static void sig_quit(int signo) {
	exit(0);
}


int main(void)
{
	struct timeval timebuf;
	struct tm* tmp;
	int count = 0;
	char buf[64];

	if (signal(SIGQUIT, sig_quit) == SIG_ERR)
		err_sys("signal error\n");

	printf("start:\n");//若在sleep之前不分配流可能会导致后面的printf无法输出？？
	while (1) {
		++count;
		Sleep(12);
		if (count % 5 == 0) {
			gettimeofday(&timebuf, NULL);
			if ((tmp = localtime(&timebuf.tv_sec)) == NULL)
				err_sys("localtime error\n");
			strftime(buf, 64, "%Y-%m-%d %H:%M:%S", tmp);
			printf("Current time: %s, tm_sec: %d\n", buf,tmp->tm_sec);
			//之所以没有\n就可能出现出输出的情况，是因为指向终端的流是行缓冲😂，这都忘了！
		}
	}

	exit(EXIT_SUCCESS);
}
