#include"../include/MyAPUE.h"

typedef int comp(const int*, const int*);

static void swap(int* lhs, int* rhs) {
	int tmp = *lhs;
	*lhs = *rhs;
	*rhs = tmp;
}

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
