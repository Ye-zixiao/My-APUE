#include"../include/MyAPUE.h"

#define LOOPTIME 100

void* thread_func(void* args) {
	/*在当前线程获得FILE对象相关联的锁后，该线程可以保证
		只有线程自身能够有机会向该FILE对象输出数据*/
	flockfile(stdout);
	//flockfile(stdout);
	for (int i = 0; i < LOOPTIME; ++i)
		printf("thread(0x%lx) output %2d\n", pthread_self(), i);
	//funlockfile(stdout);
	funlockfile(stdout);

	return (void*)NULL;
}


int main(int argc,char*argv[])
{
	int err;
	int tc = 5;
	if (argc == 2)
		tc = atoi(argv[1]);

	for (int i = 0; i < tc; i++) {
		if ((err = makeDetachedThread(thread_func, (void*)NULL)) != 0)
			err_exit(err, "makeDetachedThread error");
	}
	sleep(1);
	exit(EXIT_SUCCESS);
}


/*
在书中描述道：要求所有操作FILE对象的I/O例程的动作行为必须看起来就像是它们内部调用了flockfile和funlockfile。
所以我们可以料想，即使多个线程在使用I/O函数，这个规定或者说特性可以保证线程每一次都会原子的输入或者输出完成的
数据。
	下面是我认为的printf的大致内部结构：
	int printf(...){
		flockfile(stdout);
		output data to stdout buffer;
		funlockfile(stdout);
	}

因为这个与FILE对象相关的锁是开放给应用程序的，所以为了避免在使用标准I/O函数时发生死锁现象，
应该将这种锁采用为递归锁

*/
