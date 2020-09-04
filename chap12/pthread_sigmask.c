#include"../include/MyAPUE.h"

int quitflag;
sigset_t mask;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitloc = PTHREAD_COND_INITIALIZER;


/*
	专门处理信号的线程
*/
void* thread_func(void* args) {
	int err, signo;

	while (1) {
		//解除对SIGINT/SIGQUIT信号的阻塞，并处理这两个信号
		if ((err = sigwait(&mask, &signo)) != 0)
			err_exit(err, "sigwait error");
		switch (signo) {
		case SIGINT:
			printf("interrupt\n");
			break;
		case SIGQUIT:
			pthread_mutex_lock(&lock);
			quitflag = 1;
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&waitloc);
			return (void*)NULL;
		default:
			printf("unexpected signal %d\n", signo);
			exit(EXIT_FAILURE);
		}
	}
}

int main(void)
{
	pthread_t tid;
	sigset_t oldmask;
	int err;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0)
		err_exit(err, "pthread_sigmask");

	//新建线程会继承创建者线程的信号屏蔽字
	Pthread_create(&tid, NULL, thread_func, (void*)NULL);

	pthread_mutex_lock(&lock);
	while (quitflag == 0)
		pthread_cond_wait(&waitloc, &lock);
	pthread_mutex_unlock(&lock);
	quitflag = 0;

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("sigprocmask error\n");
	exit(EXIT_SUCCESS);
}
