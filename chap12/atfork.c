#include"../include/MyAPUE.h"
#include<pthread.h>

pthread_mutex_t lock1;
pthread_mutex_t lock2;

void prepare(void) {
	int err;

	printf("preparing locks...\n");
	if ((err = pthread_mutex_lock(&lock1)) != 0)
		err_exit(err, "can't lock lock1 in prepare handler");
	if ((err = pthread_mutex_lock(&lock2)) != 0)
		err_exit(err, "can't lock lock2 in prepare handler");
	printf("all locks locked\n");
}

void parent(void) {
	int err;

	printf("parent unlocking locks...\n");
	if ((err = pthread_mutex_unlock(&lock1)) != 0)
		err_exit(err, "can't unlock lock1 in parent handler");
	if ((err = pthread_mutex_unlock(&lock2)) != 0)
		err_exit(err, "can't unlock lock2 in parent handler");
	printf("parent unlocks all locks\n");
}

void child(void) {
	int err;

	printf("child unlocking locks...\n");
	if ((err = pthread_mutex_unlock(&lock1)) != 0)
		err_exit(err, "can't unlock lock1 in child handler");
	if ((err = pthread_mutex_unlock(&lock2)) != 0)
		err_exit(err, "can't unlock lock2 in child handler");
	printf("child unlocks all locks\n");
}

void* thread_func(void* args) {
	printf("thread(0x%lx) begin\n", pthread_self());
	pthread_mutex_lock(&lock1);
	sleep(1);
	pthread_mutex_unlock(&lock1);
	printf("thread(0x%lx) end\n", pthread_self());

	return (void*)NULL;
}

int main(void)
{
	int err;
	pid_t pid;
	pthread_t tid;

	/*
		fork处理程序可以使得当某一个线程需要fork出一个新进程的时候，必须执行pthread_atfork中
		注册的fork处理程序，先对进程中所有的互斥量进行加锁，然后再fork子进程，等创建完成之后父
		子进程都知道需要对进程中所有的锁进行解锁，这样就避免了子进程不知道需要对哪些锁进行解锁
		操作的麻烦
	*/
	if ((err = pthread_atfork(prepare, parent, child)) != 0)
		err_exit(err, "pthread_atfork error");
	if ((err = pthread_create(&tid, NULL, thread_func, (void*)NULL)) != 0)
		err_exit(err, "pthread_create error");

	sleep(1);
	printf("parent about to fork...\n");

	if ((pid = fork()) < 0)
		err_sys("fork failed\n");
	else if (pid == 0)
		printf("child return from fork\n");
	else
		printf("parent returned from fork\n");
	printf("process(%d) end\n", getpid());

	exit(EXIT_SUCCESS);
}
