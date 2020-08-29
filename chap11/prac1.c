#include"../include/MyAPUE.h"

typedef struct foo {
	int a, b, c, d;
}foo;

void printfoo(const char* msg, const struct foo* fp) {
	printf("%s", msg);
	printf(" sturcture at 0x%lx\n", (unsigned long)fp);
	printf(" foo.a = %d\n", fp->a);
	printf(" foo.b = %d\n", fp->b);
	printf(" foo.c = %d\n", fp->c);
	printf(" foo.d = %d\n", fp->d);
}

void* thread_func1(void* args) {
	foo* pret;
	
	if((pret=malloc(sizeof(foo)))==NULL)
			err_sys("malloc error\n");
	pret->a = 1; pret->b = 2;
	pret->c = 3; pret->d = 4;
	printfoo("thread 1:\n", pret);
	pthread_exit((void*)pret);
}

void* thread_func2(void* args) {
	printf("thread 2: ID is %lu\n", (unsigned long)pthread_self());
	pthread_exit((void*)NULL);
}

int main(void)
{
	pthread_t thd1, thd2;
	foo* pf;

	Pthread_create(&thd1, NULL, thread_func1, (void*)NULL);
	Pthread_join(thd1, (void*)&pf);
	sleep(1);
	printf("parent starting second thread\n");

	Pthread_create(&thd2, NULL, thread_func2, (void*)NULL);
	sleep(1);
	printfoo("parent:\n", pf);
	Pthread_join(thd2, NULL);
	free(pf);

	exit(EXIT_SUCCESS);
}
