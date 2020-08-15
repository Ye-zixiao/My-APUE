#include"../include/MyAPUE.h"
#include<pthread.h>

#define ARRSIZE 4

typedef struct foo {
	int arr[ARRSIZE];
}foo;

void pr_foo(const char* s, const foo* fp) {
	printf("%s structure at 0x%lx\n", s, (unsigned long)fp);
	for (int i = 0; i < ARRSIZE; ++i)
		printf(" foo.arr[%d] = %d\n", i, fp->arr[i]);
}

void* thread_func1(void* args) {
	//foo f = { 1,2,3,4 };//错误的使用线程栈中的局部变量进行返回
	/*pr_foo("thread 1:\n", &f);
	pthread_exit((void*)&f);*/

	foo* f = malloc(sizeof(foo));
	for (int i = 0; i < ARRSIZE; ++i)
		f->arr[i] = i + 1;
	pr_foo("thread 1:\n", f);
	pthread_exit((void*)f);
}

void* thread_func2(void* args) {
	printf("thread 2:ID is 0x%lx\n", pthread_self());
	pthread_exit((void*)NULL);
}


int main(void)
{
	int err;
	foo* fp;
	pthread_t thread_id1, thread_id2;

	if ((err = pthread_create(&thread_id1, NULL, thread_func1, NULL))
		!= 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_join(thread_id1, (void*)&fp)) != 0)
		err_exit(err, "pthread_join error");

	Sleep(1);
	putchar('\n');
	if ((err = pthread_create(&thread_id2, NULL, thread_func2, NULL))
		!= 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_join(thread_id2, NULL)) != 0)
		err_exit(err, "pthread_join error");
	pr_foo("parent:\n", fp);
	free(fp);
	
	exit(EXIT_SUCCESS);
}
