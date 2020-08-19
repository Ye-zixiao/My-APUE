#include"../include/MyAPUE.h"
#include<limits.h>

#define NTHR 4
#define NUMNUM 1000000L
#define TNUM (NUMNUM/NTHR)	//每一个线程计算的数字量

long nums[NUMNUM];
long snums[NUMNUM];

pthread_barrier_t barrier;

extern int heapsort(void*, size_t, size_t, int(*)(const void*, const void*));

int complong(const void* lhs, const void* rhs) {
	long left = *(long*)lhs;
	long right = *(long*)rhs;

	return left == right ? 0 :
		left > right ? 1 : -1;
}

void* thread_func(void* args) {
	long index = (long)args;

	heapsort(&nums[index], TNUM, sizeof(long), complong);
	if (PTHREAD_BARRIER_SERIAL_THREAD ==
		pthread_barrier_wait(&barrier))
		printf("Thread(ox%lx) is main controlling thread\n",
			pthread_self());
	return (void*)NULL;
}

void merge() {
	long index[NTHR];
	long i, minidx, sidx, num;

	for (sidx = 0; sidx < NUMNUM; sidx++) {
		num = LONG_MAX;
		for (i = 0; i < NTHR; i++) {
			if ((index[i] < (i + 1) * TNUM) && (nums[index[i]] < num)) {
				num = nums[index[i]];
				minidx = i;
			}
		}
		snums[sidx] = nums[index[minidx]];
		index[minidx]++;
	}
}

int main(void)
{
	unsigned long i;
	struct timeval start, end;
	long long startusec, endusec;
	pthread_t tid;

	srandom(time(NULL));
	for (unsigned long i = 0; i < NUMNUM; ++i)
		nums[i] = random();

	gettimeofday(&start, NULL);
	pthread_barrier_init(&barrier, NULL, NTHR + 1);
	for (i = 0; i < NTHR; i++) {
		Pthread_create(&tid, NULL, thread_func, (void*)(i * TNUM));
	}
	if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
		printf("Main thread test PTHREAD_BARRIER_SERIAL_THREAD\n");
	merge();
	gettimeofday(&end, NULL);

	startusec = start.tv_sec * 1000000 + start.tv_usec;
	endusec = end.tv_sec * 1000000 + end.tv_usec;
	printf("sort took %.4f seconds\n", (double)(endusec - startusec) / 1000000.0);
	for (i = 0; i < NUMNUM; ++i)
		printf("%ld\n", snums[i]);
	exit(EXIT_SUCCESS);

}
