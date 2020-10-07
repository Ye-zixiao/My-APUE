#include "../include/MyAPUE.h"
#include <string.h>

#define BSZ 256

int main(void)
{
	int fds[2];
	int ret, sum = 0;
	char buf[BSZ];

	memset(buf, (int)'a', sizeof(char) * BSZ);
	if (pipe(fds) == -1)
		err_sys("pipe error");
	if (set_fl(fds[1], O_NONBLOCK) == -1)
		err_sys("set_fl error");

	//每一次写入一个字节也是可以的
	while ((ret = write(fds[1], buf, BSZ)) > 0) {
		sum += ret;
		printf("write %d chars to pipe buffer\n", ret);
	}
	printf("nonblock write set errno: %s\n", strerror(errno));
	printf("pipe buffer size: %d\n", sum);

	exit(EXIT_SUCCESS);
}
