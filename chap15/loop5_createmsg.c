#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define LOOPTIME 5
#define IPC_PERM 0666
#define DEFAULT_PATH	 "/tmp"
#define DEFAULT_PRJID 32
#define MSGSIZE 256

struct mymsg {
	long mtype;
	char mtext[MSGSIZE];
};

int main(void)
{
	struct mymsg msgbuf;
	int msqid;
	key_t key;

	if ((key = ftok(DEFAULT_PATH, DEFAULT_PRJID)) == -1)
		err_sys("ftok error");

	msgbuf.mtype = 1;
	strcpy(msgbuf.mtext, "talk is cheap, show me the code!");
	for (int i = 0; i < LOOPTIME; ++i) {
		if ((msqid = msgget(key, IPC_CREAT | IPC_EXCL | IPC_PERM)) == -1)
			err_sys("msgget error");
		printf("Message Queue(id: %d) has been created.\n", msqid);
		if (msgctl(msqid, IPC_RMID, NULL) == -1)
			err_sys("msgctl error");
	}
	for (int i = 0; i < LOOPTIME; ++i) {
		if ((msqid = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | IPC_PERM)) == -1)
			err_sys("msgget error");
		printf("Message Queue(id: %d) has been created.\n", msqid);
		if (msgsnd(msqid, &msgbuf, sizeof(msgbuf.mtext), 0) == -1)
			err_sys("msgsnd error");
	}

	exit(EXIT_SUCCESS);
}

/*

实验结果：

$ ./main
Message Queue(id: 16) has been created.
Message Queue(id: 17) has been created.
Message Queue(id: 18) has been created.
Message Queue(id: 19) has been created.
Message Queue(id: 20) has been created.
Message Queue(id: 21) has been created.
Message Queue(id: 22) has been created.
Message Queue(id: 23) has been created.
Message Queue(id: 24) has been created.
Message Queue(id: 25) has been created.

# yexinhai @ Localhost in ~/projects/APUE [17:01:41]
$ cat /proc/sysvipc/msg
	   key      msqid perms      cbytes       qnum lspid lrpid   uid   gid  cuid  cgid      stime      rtime      ctime
		 0         21   666         256          1  6834     0  1000  1000  1000  1000 1603357301          0 1603357301
		 0         22   666         256          1  6834     0  1000  1000  1000  1000 1603357301          0 1603357301
		 0         23   666         256          1  6834     0  1000  1000  1000  1000 1603357301          0 1603357301
		 0         24   666         256          1  6834     0  1000  1000  1000  1000 1603357301          0 1603357301
		 0         25   666         256          1  6834     0  1000  1000  1000  1000 1603357301          0 1603357301

可以看到创建的消息队列的标识符并不会像文件描述符一样每一次都会重新使用上一次删除的哪些，
而是通过递增的方式进行使用。同时我们还可以看到通过IPC_PRIVATE创建且没有删除的那些消息队列
即使在进程结束之后仍然存在，且cbytes指出消息队列中仍然存放着消息！
*/
