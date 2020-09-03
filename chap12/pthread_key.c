#include"../include/MyAPUE.h"
#include<string.h>

//#define DEBUG
#define MAXSTRINGSZ 1024

static pthread_key_t key;
static pthread_once_t once_done;
pthread_mutex_t env_lock = PTHREAD_MUTEX_INITIALIZER;

extern char** environ;

static void Free(void* p) {
	printf("free\n");
	free(p);
}

static void key_init(void) {
#ifdef DEBUG
	pthread_key_create(&key, Free);
#else
	pthread_key_create(&key, free);
#endif
}


/*
	具有线程安全但不具有异步信号安全性的getenv函数实现，
	之所以不具有异步信号安全性是由于malloc本身就不是异步
	信号安全函数
*/
char* Getenv(const char* name) {
	size_t len;
	char* envbuf;

	pthread_once(&once_done, key_init);
	pthread_mutex_lock(&env_lock);
	if ((envbuf = pthread_getspecific(key)) == NULL) {
#ifdef DEBUG
		printf("thread(0x%lx) malloc and set specific key\n", pthread_self());
#endif
		//线程创建一个线程私有数据然后与键key相关联
		envbuf = malloc(sizeof(char) * MAXSTRINGSZ);
		if (envbuf == NULL) {
			pthread_mutex_unlock(&env_lock);
			return NULL;
		}
		pthread_setspecific(key, envbuf);
	}

	len = strlen(name);
	for (int i = 0; environ[i] != NULL; ++i) {
		if (strncmp(name, environ[i], len) == 0 &&
			environ[i][len] == '=') {
			strncpy(envbuf, &environ[i][len + 1], MAXSTRINGSZ);
			pthread_mutex_unlock(&env_lock);
			return envbuf;
		}
	}
	pthread_mutex_unlock(&env_lock);
	return NULL;
}


void* thread_func(void* args) {
	const char* name = (const char*)args;

	for (int i = 0; i < 100; i++)
		printf("thread(0x%lx): %s = %s\n", pthread_self(),
			name, Getenv(name));
	pthread_exit((void*)NULL);
}


int main(int argc, char* argv[])
{
	if (argc < 2)
		err_sys("usage: find <NAME>\n");

	for (int i = 1; i < argc; ++i)
		makeDetachedThread(thread_func, (void*)argv[i]);
	sleep(1);
	exit(EXIT_SUCCESS);
}
