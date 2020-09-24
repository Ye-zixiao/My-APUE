/**
 * 注意： 
 * 1.当需要从小到大的顺序时，将greater传入排序函数
 * 2.当需要从大到小的顺序时，将shorter传入排序函数
 */
#include"../include/MyAPUE.h"


static inline
void swap(int* lhs, int* rhs) {
	int tmp = *lhs;
	*lhs = *rhs;
	*rhs = tmp;
}



int greater(const int* lhs, const int* rhs) {
	return *lhs > * rhs ? 1 : 0;
}

int shorter(const int* lhs, const int* rhs) {
	return *lhs < *rhs ? 1 : 0;
}


/**
 * 暴力排序
 */
void BFSort(int arr[], int n, _Comp* pf) {
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; j++)
			if (pf(&arr[i], &arr[j]) > 0)
				swap(&arr[i], &arr[j]);
	}
}


/**
 * 冒泡排序
 */
static void bubble(int arr[], int n, _Comp* pf) {
	for (int i = 0; i < n - 1; ++i) {
		if (pf(&arr[i], &arr[i + 1]) > 0)
			swap(&arr[i], &arr[i + 1]);
	}
}

void BubbleSort(int arr[], int n, _Comp* pf) {
	for (int i = n; i > 0; --i) {
		bubble(arr, i, pf);
	}
}


/**
 * 选择排序
 */
static int* _select(int arr[], int n, _Comp* pf) {
	int min_max = arr[0];
	int pos = 0;

	for (int i = 1; i < n; ++i) {
		if (pf(&min_max, &arr[i]) > 0) {
			min_max = arr[i];
			pos = i;
		}
	}
	return &arr[pos];
}

void SelectSort(int arr[], int n, _Comp* pf) {
	for (int i = 0; i < n; ++i)
		swap(&arr[i], _select(&arr[i], n - i, pf));
}


/**
 * 插入排序
 */
static void _insert(int arr[], int n, _Comp* pf) {
	int key = arr[n - 1];
	int i = n - 2;

	for (; i >= 0; i--) {
		if (pf(&key, &arr[i]) > 0) break;
		arr[i + 1] = arr[i];
	}
	arr[i + 1] = key;
}

void InsertionSort(int arr[], int n, _Comp* pf) {
	for (int i = 2; i <= n; i++) {
		_insert(arr, i, pf);
	}
}


/**
 * 希尔排序
 */
void ShellSort(int arr[], int n, _Comp* pf) {
	int j, key, stride = 1;

	while (stride < n / 3)
		stride = 3 * stride + 1;
	for (; stride > 0; stride /= 3) {
		//对间隔为stride的子数组进行插入排序
		for (int i = stride; i < n; ++i) {
			key = arr[i];
			for (j = i; j >= stride && pf(&arr[j - stride], &key); j -= stride)
				arr[j] = arr[j - stride];
			arr[j] = key;
		}
	}
}

/**
 * 堆排序
 */
static void heapify(int tree[], int n, int i, _Comp* pf) {
	if (i >= n)return;

	int max = i;
	int c1 = i * 2 + 1;
	int c2 = i * 2 + 2;
	if (c1 < n && pf(&tree[c1], &tree[max]))
		max = c1;
	if (c2 < n&& pf(&tree[c2], &tree[max]))
		max = c2;
	if (max != i) {
		swap(&tree[i], &tree[max]);
		heapify(tree, n, max, pf);
	}
}

static void build_heap(int arr[], int n, _Comp* pf) {
	int last_node = n - 1;
	for (int i = (last_node - 1) / 2; i >= 0; --i) {
		heapify(arr, n, i, pf);
	}
}

void HeapSort(int arr[], int n, _Comp* pf) {
	build_heap(arr, n, pf);
	for (int i = n - 1; i > 0; --i) {
		swap(&arr[i], &arr[0]);
		heapify(arr, i, 0, pf);
	}
}


/**
 * 归并排序
 */
void mergeArr(int arr[], int left, int mid, int right, _Comp* pf) {
	int LSIZE = mid - left;
	int RSIZE = right - mid + 1;
	int larr[LSIZE];
	int rarr[RSIZE];

	for (int i = 0; i < LSIZE; ++i)
		larr[i] = arr[left + i];
	for (int i = 0; i < RSIZE; ++i)
		rarr[i] = arr[mid + i];

	int i = 0, j = 0, k = 0;
	while (i < LSIZE && j < RSIZE) {
		if (pf(&rarr[j], &larr[i]) > 0)
			arr[left + k++] = larr[i++];
		else
			arr[left + k++] = rarr[j++];
	}
	while (i < LSIZE)
		arr[left + k++] = larr[i++];
	while (j < RSIZE)
		arr[left + k++] = rarr[j++];
}

static
void RecMergeSort(int arr[], int left, int right, _Comp* pf) {
	if (left == right)return;
	int MID = (left + right) / 2;
	RecMergeSort(arr, left, MID, pf);
	RecMergeSort(arr, MID + 1, right, pf);
	mergeArr(arr, left, MID + 1, right, pf);
}

void MergeSort(int arr[], int n, _Comp* pf) {
	RecMergeSort(arr, 0, n - 1, pf);
}


/**
 * 快速排序
 */
static int partition(int arr[], int low, int high, _Comp* pf) {
	int i = low, j = high + 1;
	int key = arr[low];

	while (1) {
		while (pf(&key, &arr[++i]))
			if (i == high)break;
		while (pf(&arr[--j], &key))
			if (j == low)break;
		if (i >= j)break;
		swap(&arr[i], &arr[j]);
	}
	swap(&arr[low], &arr[j]);
	return j;
}

static void RecQuitSort(int arr[], int low, int high, _Comp* pf) {
	//若需要排序的数组长度小于10，就以插入排序进行
	if (high - low < 10) {
		InsertionSort(&arr[low], high - low + 1, pf);
	}
	else {
		int key_pos = partition(arr, low, high, pf);
		RecQuitSort(arr, low, key_pos - 1, pf);
		RecQuitSort(arr, key_pos + 1, high, pf);
	}
}

void QuitSort(int arr[], int n, _Comp* pf) {
	RecQuitSort(arr, 0, n - 1, pf);
}
