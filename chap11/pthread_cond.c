#include"../include/MyAPUE.h"
#define MAX 20

typedef struct queue {
	pthread_mutex_t lock;
	pthread_cond_t consumer;
	pthread_cond_t producer;
	int buf[MAX];//这里可以用struct job来代替
	int head;
	int tail;
	int size;
}queue;


void queue_init(queue* q) {
	q->head = 0;
	q->tail = 0;
	q->size = 0;
	if (pthread_mutex_init(&q->lock, NULL) != 0)
		err_sys("pthread_mutex_init error\n");
	if (pthread_cond_init(&q->consumer, NULL) != 0)
		err_sys("pthread_cond_init error\n");
	if (pthread_cond_init(&q->producer, NULL) != 0)
		err_sys("pthread_cond_init error\n");
}


void queue_destroy(queue* q) {
	int err;

	if ((err = pthread_mutex_destroy(&q->lock)) != 0)
		err_exit(err, "pthread_mutex_destroy error");
	if ((err = pthread_cond_destroy(&q->consumer)) != 0)
		err_exit(err, "pthread_cond_destroy error");
	if ((err = pthread_cond_destroy(&q->producer)) != 0)
		err_exit(err, "pthread_cond_destroy error");
}


void enqueue(queue* q, int ival) {
	q->buf[q->tail] = ival;
	q->tail = (q->tail + 1) % MAX;
	q->size++;
}


int dequeue(queue* q) {
	int ret;

	ret = q->buf[q->head];
	q->head = (q->head + 1) % MAX;
	q->size--;
	return ret;
}

int full(queue* q) {
	return q->size == MAX;
}

int empty(queue* q) {
	return q->size == 0;
}

void* consumer(void* args) {
	queue* q = (queue*)args;
	struct timespec timebuf;
	int value;

	while (1) {
		pthread_mutex_lock(&q->lock);
		while (empty(q)) {
			get_abstime(&timebuf, 5);

			if (pthread_cond_timedwait(&q->consumer, &q->lock, &timebuf)
				== ETIMEDOUT) {
				pthread_mutex_unlock(&q->lock);
				printf("Thread(0x%lx) quit\n", pthread_self());
				pthread_exit((void*)NULL);
			}
		}
		value = dequeue(q);

		printf("Thread(0x%lx) pop value %d\n", pthread_self(), value);
		pthread_mutex_unlock(&q->lock);
		pthread_cond_signal(&q->producer);//放在unlock的上面也可以但效果不好
	}
}


int main(int argc, char* argv[])
{
	pthread_t tid1, tid2;
	int looptime = 10;
	queue* q;

	if (argc > 1)
		looptime = atoi(argv[1]);

	if ((q = malloc(sizeof(queue))) == NULL)
		err_sys("malloc error\n");
	queue_init(q);
	Pthread_create(&tid1, NULL, consumer, (void*)q);
	Pthread_create(&tid2, NULL, consumer, (void*)q);

	for (int i = 0; i < looptime; i++) {
		pthread_mutex_lock(&q->lock);
		while (full(q))
			pthread_cond_wait(&q->producer, &q->lock);
		enqueue(q, i);
		
		/*
				之所以把printf也放在这里是因为这样push-pop的顺序
			会跟实际的push-pop的顺序是一致的，且线程们都是使用同一
			个进程的标准输出
		*/
		printf("Main thread push value %d\n", i);
		pthread_mutex_unlock(&q->lock);
		pthread_cond_signal(&q->consumer);
	}
	Pthread_join(tid1, NULL);
	Pthread_join(tid2, NULL);
	queue_destroy(q);
	free(q);

	exit(EXIT_SUCCESS);
}
