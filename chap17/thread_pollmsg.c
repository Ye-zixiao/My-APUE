#include "../include/MyAPUE.h"
#include <string.h>
#include <poll.h>
#include <sys/msg.h>

#define IPC_PERM 0666
#define NUMQUEUE 3
#define MSGSIZE 512
#define KEY 0X123

struct MyMsg {
	long mtype;
	char mtext[MSGSIZE + 1];
};


struct ThreadInfo {
	int msqid;
	int fd;
	int len;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	struct MyMsg msg;
};


//每一个辅助线程和主线程共同管理这个ThreadInfo结构体对象
struct ThreadInfo ti[NUMQUEUE];


void sig_term(int signo) {
	for (int i = 0; i < NUMQUEUE; ++i) {
		if (msgctl(ti[i].msqid, IPC_RMID, NULL) == -1)
			err_sys("msgctl error");
		printf("Message Queue(id %d) has been removed\n", ti[i].msqid);
	}
	exit(EXIT_SUCCESS);
}


void* helper(void* args) {
	struct ThreadInfo* tip = args;
	int nrecv;

	for (;;) {
		memset(&tip->msg, 0, sizeof(struct MyMsg));
		if ((nrecv = msgrcv(tip->msqid, &tip->msg, sizeof(tip->msg.mtext),
				0, MSG_NOERROR)) == -1)
			err_sys("msgrcv error");
		tip->len = nrecv;
		/* 当辅助线程接收到消息之后就加锁试图通知主线程处理获取的数据，
			期间辅助线程进入休眠状态，由主线程在完成数据处理之后唤醒 */
		pthread_mutex_lock(&tip->lock);
		if (write(tip->fd, "a", sizeof(char)) < 0)
			err_sys("write error");
		pthread_cond_wait(&tip->cond, &tip->lock);
		pthread_mutex_unlock(&tip->lock);
	}
}


int main(void)
{
	struct pollfd pfds[NUMQUEUE];
	pthread_t tid[NUMQUEUE];
	int fds[2];
	char ch;
	int err, nread;

	if (signal(SIGTERM, sig_term) == SIG_ERR)
		err_sys("signal error");

	for (int i = 0; i < NUMQUEUE; ++i) {
		if ((ti[i].msqid = msgget(KEY + i, IPC_CREAT | IPC_EXCL | IPC_PERM)) == -1)
			err_sys("msgget error");
		printf("Message Queue(id %d) has been created\n", ti[i].msqid);
		fflush(stdout);

		/*if (fd_pipe(fds) == -1)
			err_sys("fd_pipe error");*/
		if (pipe(fds) == -1)
			err_sys("pipe error");
		pfds[i].fd = fds[0];
		pfds[i].events = POLLIN;
		ti[i].fd = fds[1];
		if (pthread_mutex_init(&ti[i].lock, NULL) == -1)
			err_sys("pthread_mutex_init error");
		if (pthread_cond_init(&ti[i].cond, NULL) == -1)
			err_sys("pthread_cond_init error");
		if ((err = pthread_create(&tid[i], NULL, helper, (void*)&ti[i])) == -1)
			err_exit(err, "pthread_create error");
	}

	for (;;) {
		if (poll(pfds, NUMQUEUE, -1) == -1)
			err_sys("poll error");

		for (int i = 0; i < NUMQUEUE; ++i) {
			if (pfds[i].revents & POLLIN) {
				/* 当辅助线程通知主线程有可用消息数据需要处理时，主线程就可以
					从TreadInfo结构体对象中获取相应的数据。cond和lock成员的主
					要目的是为了协调线程之间的同步（在其中一方完成数据处理工作
					之后允许另一方开始它自己的操作） */
				pfds[i].revents = 0;
				if ((nread = read(pfds[i].fd, &ch, sizeof(char))) < 0)
					err_sys("read error");
				ti[i].msg.mtext[ti[i].len] = 0;
				printf("Message Queue(id %d) received message:\n\t%s\n", ti[i].msqid, ti[i].msg.mtext);
				pthread_mutex_lock(&ti[i].lock);
				pthread_cond_signal(&ti[i].cond);
				pthread_mutex_unlock(&ti[i].lock);
			}
		}
	}

	exit(EXIT_SUCCESS);
}
