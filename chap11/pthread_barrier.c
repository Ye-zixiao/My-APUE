#include"../include/MyAPUE.h"
#include<limits.h>

#define DEBUG
#define NTHR 4	//更改次数来线程数的改变，通过实验知道多线程可以比单线程（不包括main线程）具有更高的效率
#define NUMNUM 10000000
#define TNUM (NUMNUM/NTHR)

pthread_barrier_t barrier;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int comp(const int* lhs, const int* rhs) {
	return *lhs > * rhs ? 1 : 0;
}


/*
	把一个大数组分成NTHR个线程数组，然后每一个线程数组做heap_sort，
	完成后把每一个以及有序的线程数组合并到一个大的目标数组，使之有序

	线程0 线程1 线程2 线程3
	1 2 | 5 6 | 3 4 | 7 8	局部有序
			  |
			  |合并成
			  ↓
	1 2   3 4   5 6  7  8	整体有序

*/
void merge(const int* src_arr, int* tar_arr) {
	int thread_index[NTHR];
	int min_t;
	int min;

	//初始化各个线程下标数组的最初的下标
	for (int i = 0; i < NTHR; ++i)
		thread_index[i] = i * TNUM;
	for (int i = 0; i < NUMNUM; ++i) {
		min = INT_MAX;
		for (int j = 0; j < NTHR; ++j) {

			/*	1、线程下标数组中的下标没有越界
				2、找出这一轮中最小的数赋给min，它会赋给目标数组
					以及这个最小值来自哪个min_t线程数组
			*/
			if ((thread_index[j] < (j + 1) * TNUM) && (src_arr[thread_index[j]] < min)) {
				min = src_arr[thread_index[j]];
				min_t = j;
			}
		}
		tar_arr[i] = src_arr[thread_index[min_t]];
		thread_index[min_t]++;
	}
}


void* thread_func(void* args) {
	int* arr = (int*)args;

	heap_sort(arr, TNUM, comp);
	pthread_barrier_wait(&barrier);
	
	return (void*)NULL;
}


int main(void)
{
	struct timespec start, end;
	int* src_arr, *tar_arr;
	pthread_t tid;

	//随机初始化一个数组
	srand(time(NULL));
	src_arr = malloc(sizeof(int) * NUMNUM);
	tar_arr = malloc(sizeof(int) * NUMNUM);
	for (int i = 0; i < NUMNUM; ++i)
		src_arr[i] = rand() % NUMNUM;

	clock_gettime(CLOCK_REALTIME, &start);

	//让各线程分而治之处理属于自己的小数组，main线程做合并工作
	pthread_barrier_init(&barrier, NULL, NTHR + 1);
	for (int i = 0; i < NTHR; ++i)
		Pthread_create(&tid, NULL, thread_func, (void*)&src_arr[i*TNUM]);
	pthread_barrier_wait(&barrier);
	merge(src_arr, tar_arr);

	clock_gettime(CLOCK_REALTIME, &end);

	//打印输出
	long nas = (end.tv_sec - start.tv_sec) * 1000000000 +
		end.tv_nsec - start.tv_nsec;
	for (int i = 0; i < NUMNUM; i++)
		printf("%d\n", tar_arr[i]);
	printf("sort took %.4f seconds\n", (double)nas / 1000000000.0);

	free(src_arr);
	free(tar_arr);
	exit(0);
}
