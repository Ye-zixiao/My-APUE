/**
 * Message Queue Server
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

	//服务器创建一个新的ipc结构
	if ((key = ftok(DEFAULT_PATH, DEFAULT_PROID)) == -1)
		err_sys("ftok error");
	while ((msqid = msgget(key, IPC_CREAT | DEFAULT_PERM)) == -1) {
		if ((msqid = msgget(key, 0)) == -1)
			err_sys("msgget error");
		if (msgctl(msqid, MSG_STAT, NULL) == -1)
			err_sys("msgctl error");
	}

	while (msgrcv(msqid, (void*)&msg, sizeof(msg.mdata), 0, MSG_NOERROR) != -1) {
		//在接收到终止服务器字符串后，开始执行终止处理
		if (strncmp(msg.mdata, "server end", 10) == 0) {
			fputs(msg.mdata, stderr);
			msgsnd(msqid, (void*)&msg, sizeof(msg.mdata), 0);
			msgctl(msqid, IPC_RMID, NULL);
			break;
		}
		printf("client: %s", msg.mdata);
		fflush(stdout);
		
		strcpy(msg.mdata, "hello world from server\n");
		if (msgsnd(msqid, (void*)&msg, sizeof(msg.mdata), 0) == -1)
			err_sys("msgsnd error");
	}

	exit(EXIT_SUCCESS);
}
