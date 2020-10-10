#include "../../include/MyAPUE.h"
#include <string.h>
#include <mqueue.h>

int main(int argc, char* argv[])
{
	struct mq_attr attr;
	mqd_t mqd;

	if (argc <= 2 || strcmp(argv[1], "--help") == 0)
		err_quit("usage: chmq [-+]n <mq_name>");

	/* 获取原来的消息队列描述标志，然后在其基础上进行修改 */
	if ((mqd = mq_open(argv[2], O_RDWR)) < 0)
		err_sys("mq_open error");
	if (mq_getattr(mqd, &attr) == -1)
		err_sys("mq_getattr error");
	if (strcmp(argv[1], "-n") == 0)
		attr.mq_flags &= ~O_NONBLOCK;
	else if (strcmp(argv[1], "+n") == 0)
		attr.mq_flags |= O_NONBLOCK;
	else
		err_sys("usage: chmq [-+] <mq_name>");
	if (mq_setattr(mqd, &attr, NULL) == -1)
		err_sys("mq_setattr error");

	/* 在修改消息队列属性之后检查结果，注意：O_NONBLOCK属性是消息队列描述的标志
	，修改该标志只会影响到使用它的用户进程而对于消息队列本身是没有任何影响 */
	if (mq_getattr(mqd, &attr) == -1)
		err_sys("mq_getattr error");
	printf("Now message queue is set to: %s\n",
		(attr.mq_flags & O_NONBLOCK) == 0 ? "Block" : "NonBlock");

	exit(EXIT_SUCCESS);
}
