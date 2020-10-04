/**
 * System V(XSI) Message Queue Server
 */
#include "../include/MyAPUE.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define DEFAULT_PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
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

	//创建system v 消息队列，若该消息队列已存在则先删除原来的消息队列
	if ((mykey = ftok(DEFAULT_PATH, DEFAULT_PROID)) == -1)
		err_sys("ftok error");
	while ((msqid = msgget(mykey, IPC_CREAT | IPC_EXCL | DEFAULT_PERM)) == -1) {
		if (errno == EEXIST) {
			if ((msqid = msgget(mykey, 0)) == -1)
				err_sys("msgget error");
			if ((msgctl(msqid, IPC_RMID, NULL)) == -1)
				err_sys("msgctl error");
			fprintf(stderr, "old message queue (id=%d) removed\n", msqid);
		}
		else {
			err_sys("msgget error");
		}
	}

	//从消息队列中获取消息并对客户端进行回应
	while (msgrcv(msqid, (void*)&msg, sizeof(msg.mtext), 0, MSG_NOERROR) != -1) {
		if (strncmp(msg.mtext, "server end", 10) == 0) {
			fputs(msg.mtext, stderr);
			msgsnd(msqid, (void*)&msg, sizeof(msg.mtext), 0);
			msgctl(msqid, IPC_RMID, NULL);
			break;
		}
		printf("client: %s", msg.mtext);
		fflush(stdout);

		strcpy(msg.mtext, "hello world from server\n");
		if (msgsnd(msqid, (void*)&msg, sizeof(msg.mtext), 0) == -1)
			err_sys("msgsnd error");
	}

	exit(EXIT_SUCCESS);
}
