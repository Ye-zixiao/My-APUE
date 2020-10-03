/**
 * Message Queue Client
 */
#include "../include/MyAPUE.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define DEFAULT_PERM 0666
#define DEFAULT_PATH "/tmp"
#define DEFAULT_PROID 25
#define MSGSIZE 256

struct message {
	long mtype;
	char mdata[MSGSIZE];
} msg;


int main(void)
{
	key_t key;
	int msqid;

	//客户端负责引用一个已存在的消息队列
	if ((key = ftok(DEFAULT_PATH, DEFAULT_PROID)) == -1)
		err_sys("ftok error");
	if ((msqid = msgget(key, 0)) == -1)
		err_sys("msgget error");

	while (fgets(msg.mdata, sizeof(msg.mdata), stdin) != NULL) {
		msg.mtype = 1;
		if (msgsnd(msqid, (void*)&msg, sizeof(msg.mdata), 0) == -1)
			err_sys("msgsnd error");

		if (msgrcv(msqid, (void*)&msg, sizeof(msg.mdata), 0, MSG_NOERROR) == -1)
			err_sys("msgrcv error");
		//若接收到服务器终止的字符串，客户端也应该进行终止处理
		if (strncmp(msg.mdata, "server end", 10) == 0) {
			fputs("client end\n", stderr);
			break;
		}
		printf("server: %s", msg.mdata);
		fflush(stdout);
	}

	exit(EXIT_SUCCESS);
}
