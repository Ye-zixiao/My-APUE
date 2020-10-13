#include"../include/MyAPUE.h"

extern char* mygetenv(const char* name);

void* thread_func(void* args) {
	const char* name = (char*)args;
	int i = 100;

	while (i--)
		printf("thread(0x%lx): %s = %s\n", pthread_self(),
			name, mygetenv(name));
	return (void*)NULL;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage: find <NAME...>");

	for (int i = 1; i < argc; ++i)
		makeDetachedThread(thread_func, (void*)argv[i]);
	sleep(1);

	return 0;
}



/*
	输入：./main USER HOME 创建了两个线程，其中一个只查询HOME变量，而另一个子查询USER变量

	实验结果：
	...
	thread(0x7f37e24b5640): USER = yexinhai
	thread(0x7f37e24b5640): USER = yexinhai
	thread(0x7f37e24b5640): USER = yexinhai
	thread(0x7f37e1cb4640): HOME = yexinhai
	thread(0x7f37e1cb4640): HOME = /home/yexinhai
	thread(0x7f37e1cb4640): HOME = /home/yexinhai
	thread(0x7f37e1cb4640): HOME = /home/yexinhai
	...
	可以看到同一个线程本来应该输出同一个环境变量的结果，但却由于mygetenv不具有线程安全性导致输出了不同的结果

*/
