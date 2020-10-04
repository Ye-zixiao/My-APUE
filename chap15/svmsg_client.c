/**
 * System V(XSI) Message Queue Client
 */
#include "../include/MyAPUE.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define DEFAULT_PATH "/tmp"
#define DEFAULT_PROID 24
#define MSGSIZE 256

struct message {
	long mtype;
	char mtext[MSGSIZE];
} msg;


int main(void)
{
	key_t mykey;
	int msqid;

	//试图引用一个已经存在的消息队列
	if ((mykey = ftok(DEFAULT_PATH, DEFAULT_PROID)) == -1)
		err_sys("ftok error");
	if ((msqid = msgget(mykey, 0)) == -1)
		err_sys("msgget error");

	//从stdin获取输入并将其发送给服务端，然后从中得到回应
	while (fgets(msg.mtext, MSGSIZE, stdin) != NULL) {
		msg.mtype = 1;
		if (msgsnd(msqid, (void*)&msg, sizeof(msg.mtext), 0) == -1)
			err_sys("msgsnd error");

		if (msgrcv(msqid, (void*)&msg, sizeof(msg.mtext), 0, MSG_NOERROR) == -1)
			err_sys("msgrcv error");
		if (strncmp(msg.mtext, "server end", 10) == 0) {
			fputs("client end\n", stderr);
			break;
		}
		printf("server: %s", msg.mtext);
		fflush(stdout);
	}
	if (ferror(stdin))
		err_sys("fgets error");

	exit(EXIT_SUCCESS);
}
