/**
 * pollmsg.c：
 * 该服务进程中，通过用多个及使用消息队列和UNIX域套接字的线程来封装
 * （隐藏）对消息队列的使用，使得服务进程可以像使用文件描述符一样使用
 * 消息队列并于其他进程进行通信
 *                            write to fd-pipe	    msgrcv         消息队列0     msgsnd
 *                   /￣￣￣￣￣￣￣￣￣￣  辅助线程0 ￣￣￣￣￣￣￣￣￣▯▯▯▯￣￣￣￣￣￣客户进程0
 *                 /
 *                v read      write to fd-pipe		msgrcv         消息队列1     msgsnd
 * 服务器进程主线程 <—————————————————————  辅助线程1  —————————————————▯▯▯▯———————————客户进程1
 *        |       ʌ
 *        |        \          write to fd-pipe      msgrcv         消息队列2     msgsnd
 *        v          \_____________________ 辅助线程2  _________________▯▯▯▯___________客户进程2
 *      stdout
 * 
 */
#include "../include/MyAPUE.h"
#include <sys/msg.h>
#include <string.h>
#include <poll.h>

#define IPC_PERM 0666
#define MSGSIZE 128
#define NUMQUEUE 3
#define KEY 0x123

int msqids[NUMQUEUE];

//线程函数参数结构体
struct threadargs {
	int msqid;
	int sockfd;
};


struct mymsg {
	long mtype;
	char mtext[MSGSIZE];
};


//接收SIGTERM时的信号处理函数
void sig_term(int signo) {
	for (int i = 0; i < NUMQUEUE; ++i) {
		if (msgctl(msqids[i], IPC_RMID, 0) == -1)
			err_sys("msgctl error");
		printf("Message Queue(key: %d, id: %d) has been removed\n",
				KEY + i, msqids[i]);
	}
	exit(EXIT_SUCCESS);
}


void* threadfunc(void* args) {
	struct threadargs* targs = args;
	struct mymsg msgbuf;
	int nrecv;

	for (;;) {
		/* 从消息队列中接收消息，然后通过fd-pipe全双工管道转发给主线程 */
		memset(&msgbuf, 0, sizeof(struct mymsg));
		if ((nrecv = msgrcv(targs->msqid, &msgbuf, sizeof(msgbuf.mtext),
							0, MSG_NOERROR)) == -1)
			err_sys("msgrcv error");
		if (write(targs->sockfd, msgbuf.mtext, nrecv) != nrecv)
			err_sys("write error");
	}
}


int main(int argc, char* argv[])
{
	struct threadargs targs[NUMQUEUE];
	struct pollfd pfds[NUMQUEUE];
	pthread_t ptds[NUMQUEUE];
	int nrecv, fds[2];
	char buf[MSGSIZE];

	if (signal(SIGTERM, sig_term) == SIG_ERR)
		err_sys("signal error");

	/* 创建3个消息队列，创建3个全双工管道，初始化pollfd结构体数组和
		线程函数参数结构体数组，并在期间创建3个线程以提供消息队列到
		全双工管道队列的服务 */
	for (int i = 0; i < NUMQUEUE; ++i) {
		if ((msqids[i] = msgget(KEY + i, IPC_CREAT | IPC_EXCL | IPC_PERM)) == -1)
			err_sys("msgget error");
		printf("Create message queue(key: %d, id: %d)\n", KEY + i, msqids[i]);

		if (socketpair(AF_UNIX, SOCK_DGRAM, 0, fds) == -1)
		//if (pipe(fds) == -1) //普通的单双工管道pipe也可以完成这个数据转交的工作
			err_sys("socketpair error");
		targs[i].msqid = msqids[i];
		targs[i].sockfd = fds[1];
		pfds[i].fd = fds[0];
		pfds[i].events = POLLIN;
		if (pthread_create(&ptds[i], NULL, threadfunc, (void*)&targs[i]) == -1)
			err_sys("pthread_create error");
	}

	for (;;) {
		/* 使用poll监控套接字描述符，直到任一描述符变得可用时返回 */
		if (poll(pfds, NUMQUEUE, -1) == -1)
			err_sys("poll error");
		for (int i = 0; i < NUMQUEUE; ++i) {
			if (pfds[i].revents & POLLIN) {
				if ((nrecv = read(pfds[i].fd, buf, MSGSIZE)) == -1)
					err_sys("read error");
				buf[nrecv] = '\0';
				printf("Message Queue(key: 0x%x, id: %d) received message:\n\t%s\n",
								KEY + i, msqids[i], buf);
			}
			
		}
	}
}
