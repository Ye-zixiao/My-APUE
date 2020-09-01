#include"../include/MyAPUE.h"

//使用互斥量和条件变量实现屏障机制
typedef struct Pthread_barrier_t {
	pthread_mutex_t lock;
	pthread_cond_t cond;
	unsigned count;
}Pthread_barrier_t;


/*
	自定义屏障初始化
*/
int Pthread_barrier_init(Pthread_barrier_t* barrier, unsigned count) {
	int err;

	barrier->count = count;
	if ((err = pthread_mutex_init(&barrier->lock, NULL)) != 0)
		return err;
	if ((err = pthread_cond_init(&barrier->cond, NULL)) != 0)
		return err;
	return 0;
}


/*
	自定义屏障销毁
*/
int Pthread_barrier_destroy(Pthread_barrier_t* barrier) {
	int err;

	if ((err = pthread_cond_destroy(&barrier->cond)) != 0)
		return err;
	if ((err = pthread_mutex_destroy(&barrier->lock)) != 0)
		return err;
	return 0;
}


/*
	自定义屏障等待函数
*/
int Pthread_barrier_wait(Pthread_barrier_t* barrier) {
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
	printf("Thread(0x%lx) begin\n", pthread_self());
	sleep(1);
	Pthread_barrier_wait(&barrier);
	printf("Thread(0x%lx) end\n", pthread_self());

	pthread_exit((void*)NULL);
}


int main(int argc, char* argv[]) {
	unsigned THREAD_NUM = 4;
	pthread_t thd;

	if (argc == 2)
		THREAD_NUM = (unsigned)atoi(argv[1]);

	Pthread_barrier_init(&barrier, THREAD_NUM + 1);
	for (int i = 0; i < THREAD_NUM; ++i)
		Pthread_create(&thd, NULL, thread_func, (void*)NULL);
	Pthread_barrier_wait(&barrier);
	sleep(1);/*因为main线程在从Pthread_barrier_wait返回之后很快就会调用pthread_exit，
			 而其他线程可能还没有运行完毕，所以需要让main线程稍微等待它们一下，至少要让它们的打印要执行到*/
	Pthread_barrier_destroy(&barrier);

	exit(EXIT_SUCCESS);
}
