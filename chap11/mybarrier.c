/*
	实现自定义的Pthread_barrier_t及其相关的函数
*/

#include"../include/MyAPUE.h"

#define THREAD_NUM 4

//自定义Pthread_barrier_t结构体
typedef struct Pthread_barrier_t {
	unsigned count;
	pthread_mutex_t lock;
	pthread_cond_t cond;
}Pthread_barrier_t;


/*
	自定义屏障的初始化
*/
int Pthread_barrier_init(Pthread_barrier_t* barrier, unsigned count) {
	int err = 0;

	barrier->count = count;
	if ((err = pthread_mutex_init(&barrier->lock, NULL)) != 0)
		return err;
	if ((err = pthread_cond_init(&barrier->cond, NULL)) != 0)
		return err;
	return err;
}


/*
	自定义屏障的销毁
*/
int Pthread_barrier_destroy(Pthread_barrier_t* barrier) {
	int err = 0;

	if ((err = pthread_mutex_destroy(&barrier->lock)) != 0)
		return err;
	if ((err = pthread_cond_destroy(&barrier->cond)) != 0)
		return err;
	return err;
}


/*
		自定义屏障的屏障等待函数实现，其中该屏障等待函数并不没有像POSIX中的
	pthread_barrier_wait那样具有随机返回PTHREAD_BARRIER_SERIAL_THREAD给
	任意使用屏障的线程，而总是让最后一个调用Pthread_barrier_wait的线程获得
	该数值
*/
int	Pthread_barrier_wait(Pthread_barrier_t* barrier) {
	pthread_mutex_lock(&barrier->lock);
	if (--barrier->count == 0) {
		pthread_mutex_unlock(&barrier->lock);
		pthread_cond_broadcast(&barrier->cond);
		return PTHREAD_BARRIER_SERIAL_THREAD;
	}
	pthread_cond_wait(&barrier->cond, &barrier->lock);
	pthread_mutex_unlock(&barrier->lock);
	return 0;
}


Pthread_barrier_t barrier;


void* thread_func(void* args) {
	sleep(1);
	printf("0x%lx: before-><-\n", pthread_self());
	Pthread_barrier_wait(&barrier);
	printf("0x%lx: after-><-\n", pthread_self());
	
	pthread_exit((void*)NULL);
}


int main(void)
{
	pthread_t tid;

	Pthread_barrier_init(&barrier, THREAD_NUM + 1);
	for (int i = 0; i < THREAD_NUM; ++i)
		Pthread_create(&tid, NULL, thread_func, (void*)NULL);
	Pthread_barrier_wait(&barrier);

	/*	因为主线程并没有等待其他的线程，所以需要sleep稍微等待其他线程，
		否则会因为main线程调用exit函数而使整个进程终止*/
	sleep(1);							
	Pthread_barrier_destroy(&barrier);

	exit(EXIT_SUCCESS);
}
