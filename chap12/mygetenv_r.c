#include"../include/MyAPUE.h"

#define MYBUFSIZE 1024

extern int mygetenv_r(const char* name, char* buf, int buflen);

void* thread_func(void* args) {
	const char* name = (const char*)args;
	char buf[MYBUFSIZE];
	int err;

	for (int i = 0; i < 100; i++) {
		if ((err = mygetenv_r(name, buf, MYBUFSIZE)) != 0)
			pthread_exit((void*)NULL);
		printf("thread(0x%lx): %s = %s\n", pthread_self(), name, buf);
	}
	pthread_exit((void*)NULL);
}


int main(int argc, char* argv[]) {
	if (argc < 2)
		err_sys("usage: find <NAME...>");

	for (int i = 1; i < argc; ++i)
		makeDetachedThread(thread_func, (void*)argv[i]);
	sleep(1);

	exit(EXIT_SUCCESS);
}

