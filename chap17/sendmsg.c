/**
 * sendmsg.c：
 * 一个将消息发送给指定消息队列的客户程序
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/msg.h>
#include <poll.h>

#define MSGSIZE 128

struct mymsg {
	long mtype;
	char mtext[MSGSIZE];
};


int main(int argc, char* argv[])
{
	struct mymsg msgbuf;
	key_t key;
	int msqid;

	if (argc != 3)
		err_quit("usage: msgsend <key> <message>");

	key = strtol(argv[1], NULL, 0);
	if ((msqid = msgget(key, 0)) == -1)
		err_sys("msgget error");
	memset(&msgbuf, 0, sizeof(struct mymsg));
	strncpy(msgbuf.mtext, argv[2], MSGSIZE - 1);
	msgbuf.mtype = 1;
	if (msgsnd(msqid, &msgbuf, strlen(msgbuf.mtext), 0) == -1)
		err_sys("msgsnd error");
	
	exit(EXIT_SUCCESS);
}
