#include"../include/MyAPUE.h"


typedef int comp(const int*, const int*);

static void swap(int* lhs, int* rhs) {
	int tmp = *lhs;
	*lhs = *rhs;
	*rhs = tmp;
}


/*
	堆排序
*/
static void heapify(int tree[], int n, int i, comp*pf) {
	if (i >= n)return;

	int max = i;
	int c1 = i * 2 + 1;
	int c2 = i * 2 + 2;
	if (c1 < n && pf(&tree[c1], &tree[max]))
		max = c1;
	if (c2 < n && pf(&tree[c2], &tree[max]))
		max = c2;
	if (max != i) {
		swap(&tree[i], &tree[max]);
		heapify(tree, n, max, pf);
	}
}

static void build_heap(int arr[], int n, comp*pf) {
	int last_node = n - 1;
	for (int i = (last_node - 1) / 2; i >= 0; --i) {
		heapify(arr, n, i, pf);
	}
}

void heap_sort(int arr[], int n, comp*pf) {
	build_heap(arr, n, pf);
	for (int i = n - 1; i > 0; --i) {
		swap(&arr[i], &arr[0]);
		heapify(arr, i, 0, pf);
	}
}


/*
	冒泡排序
*/
static void bubble(int arr[], int n, comp* pf) {
	for (int i = 0; i < n-1; ++i) {
		if (pf(&arr[i], &arr[i + 1]) > 0)
			swap(&arr[i], &arr[i + 1]);
	}
}

void bubble_sort(int arr[], int n, comp* pf) {
	for (int i = n; i > 0; --i) {
		bubble(arr, i, pf);
	}
}


/*
	暴力排序
*/
void voilent_sort(int arr[], int n, comp*pf) {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; j++)
			if (pf(&arr[i], &arr[j]) > 0)
				swap(&arr[i], &arr[j]);
	}
}


/*
	选择排序
*/
static int* _select(int arr[], int n, comp* pf) {
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

void select_sort(int arr[], int n, comp* pf) {
	for (int i = n; i > 0; --i)
		swap(&arr[i - 1], _select(arr, i, pf));
}
