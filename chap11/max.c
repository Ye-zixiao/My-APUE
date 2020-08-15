#include"../include/MyAPUE.h"
#include<pthread.h>

typedef struct arg_type {
	int lvalue;
	int rvalue;
}arg_type;

void* thread_func(void* args) {
	arg_type* p = (arg_type*)args;
	printf("thread func return the max one between lvalue and rvalue\n");
	return (void*)(long)(p->lvalue > p->rvalue ? p->lvalue : p->rvalue);
	//在64位架构中：void*指针的大小是64位，long的大小也是64位
}

int main(void)
{
	pthread_t thread_id;
	arg_type argbuf;
	void* retval;
	int err;
	
	argbuf.lvalue = 10;
	argbuf.rvalue = 23;
	if ((err = pthread_create(&thread_id, NULL, thread_func, (void*)&argbuf))
		!= 0)
		err_exit(err, "thread create error");
	if ((err = pthread_join(thread_id, &retval)) != 0)
		err_exit(err, "pthread_join error");
	printf("The max one is %ld\n", (long)retval);
	//printf("sizeof long: %lu\n", sizeof(void*));

	exit(EXIT_SUCCESS);
}
