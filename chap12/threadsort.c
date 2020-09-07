#include"../include/MyAPUE.h"
#include<limits.h>

//#define DEBUG
#define NTHR 8
#define NUMNUM 10000000
#define TNUM (NUMNUM/NTHR)

pthread_barrier_t barrier;


void pr(const int* arr, int n) {
	for (int i = 0; i < n; i++)
		printf("%d\n", arr[i]);
	putchar('\n');
}

/*
	合并数组，生成整体有序的数组
*/
void merge(int dest_arr[], const int src_arr[]) {
	int thread_index[NTHR];
	int min, min_from;

	for (int i = 0; i < NTHR; ++i)
		thread_index[i] = i * TNUM;
	for (int i = 0; i < NUMNUM; ++i) {
		min = INT_MAX;
		for (int j = 0; j < NTHR; ++j) {
			if ((thread_index[j] < (j + 1) * TNUM) &&
					greater(&min, &src_arr[thread_index[j]])) {
					//(min > src_arr[thread_index[j]])) {
				min = src_arr[thread_index[j]];
				min_from = j;
			}
		}
		dest_arr[i] = min;
		thread_index[min_from]++;
	}
}



void* thread_func(void* args) {
	int* arr = (int*)args;

	heap_sort(arr, TNUM, greater);
	//voilent_sort(arr,TNUM, greater);
#ifdef DEBUG
	flockfile(stdout);
	pr(arr,TNUM);
	funlockfile(stdout);
#endif
	pthread_barrier_wait(&barrier);
	return (void*)NULL;
}


int main(void)
{
	int* src_arr, * dest_arr;
	int err;

	//初始化数组
	if ((src_arr = malloc(sizeof(int) * NUMNUM)) == NULL)
		err_sys("malloc error\n");
	if ((dest_arr = malloc(sizeof(int) * NUMNUM)) == NULL) {
		free(src_arr);
		err_sys("malloc error\n");
	}
	srand(time(NULL));
	for (int i = 0; i < NUMNUM; i++)
		src_arr[i] = rand() % NUMNUM;

	//创建线程对数组进行排序
	if ((err = pthread_barrier_init(&barrier, NULL, NTHR + 1)) != 0)
		err_exit(err, "pthread_barrier_init error");
	for (int i = 0; i < NTHR; ++i)
		makeDetachedThread(thread_func, (void*)&src_arr[i*TNUM]);
	pthread_barrier_wait(&barrier);
	merge(dest_arr, src_arr);
	printf("Result:\n");
	pr(dest_arr, NUMNUM);

	//销毁工作
	free(src_arr);
	free(dest_arr);
	exit(EXIT_SUCCESS);
}

/*
	不考虑main线程：（硬件平台4C8T）
	1、单线程：
		$ time ./main > tmp
		./main > tmp  4.17s user 0.08s system 89% cpu 4.758 total
	2、双线程：
		$ time ./main > tmp
		./main > tmp  3.99s user 0.09s system 139% cpu 2.932 total
	3、四线程：
		$ time ./main > tmp
		./main > tmp  4.15s user 0.07s system 186% cpu 2.270 total
	4、八线程：
		$ time ./main > tmp
		./main > tmp  5.30s user 0.11s system 271% cpu 1.994 total
	5、十二线程：
		$ time ./main > tmp
		./main > tmp  4.54s user 0.08s system 222% cpu 2.080 total
	6、十六线程：
		$ time ./main > tmp
		./main > tmp  4.25s user 0.07s system 185% cpu 2.326 total
	我认为之所以八线程时对程序执行效率提升相较于四线程少，可能主要是由于线程数增加之后
	main线程中合并操作的迭代次数增加有关（四线程时合并操作迭代次数是4xNUMNUM，而在八线
	程中合并操作迭代次数是8xNUMNUM）
*/
