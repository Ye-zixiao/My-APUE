/*
* 
* 细颗粒度锁：1）hashlock只负责散列表的结构组织、列表中节点的添加删除的加锁,
*				而对于foo结构体内部的count引用计数不进行管理
*			 2）而foo结构中的lock负责与自身除pnext跟节点增删无关的其他数据的加锁
* 
*/
#include"../include/MyAPUE.h"

#define NHASH 29
#define HASH(id) (((unsigned long)id)%NHASH)

struct foo {
	int f_count;
	unsigned long f_id;
	int f_value;
	pthread_mutex_t lock;
	struct foo* pnext;
};
typedef struct foo foo;

//散列表
foo* foo_hash[NHASH];
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

foo* foo_alloc(unsigned long id);
void foo_hold(foo* fp);
foo* foo_find(unsigned long id);
int foo_read(foo* fp);
void foo_update(foo* fp);
void foo_rele(foo* fp);

void* thread_func(void* args);


int main(void)
{
	pthread_t tid1, tid2, tid3, tid4;
	foo* pf1, * pf2;

	if ((pf1 = foo_alloc(32)) == NULL)
		err_sys("foo_alloc error\n");
	if ((pf2 = foo_alloc(21)) == NULL)
		err_sys("foo_alloc error\n");
	Pthread_create(&tid1, NULL, thread_func, (void*)32);
	Pthread_create(&tid2, NULL, thread_func, (void*)32);
	Pthread_create(&tid3, NULL, thread_func, (void*)21);
	Pthread_create(&tid4, NULL, thread_func, (void*)21);
	Pthread_join(tid1, NULL);
	Pthread_join(tid2, NULL);
	Pthread_join(tid3, NULL);
	Pthread_join(tid4, NULL);
	printf("\nAt the end of process: pf1->f_value = %d\n",
		foo_read(pf1));
	printf("At the end of process: pf2->f_value = %d\n",
		foo_read(pf2));

	foo_rele(pf1);
	foo_rele(pf2);
	exit(EXIT_SUCCESS);
}


foo* foo_alloc(unsigned long id) {
	foo* pret = NULL;
	unsigned long index;

	if ((pret = malloc(sizeof(foo))) != NULL) {
		pret->f_count = 1;
		pret->f_id = id;
		if (pthread_mutex_init(&pret->lock, NULL) != 0) {
			free(pret);
			return NULL;
		}
		index = HASH(id);
		pthread_mutex_lock(&hashlock);
		pret->pnext = foo_hash[index];
		foo_hash[index] = pret;
		pthread_mutex_lock(&pret->lock);
		pthread_mutex_unlock(&hashlock);
		pret->f_value = 0;
		//
		pthread_mutex_unlock(&pret->lock);
	}
	return pret;
}

void foo_hold(foo* pf) {
	pthread_mutex_lock(&pf->lock);
	pf->f_count++;
	pthread_mutex_unlock(&pf->lock);
}

foo* foo_find(unsigned long id) {
	foo* pret = NULL;

	pthread_mutex_lock(&hashlock);
	for (pret = foo_hash[HASH(id)];
		pret != NULL; pret = pret->pnext) {
		if (pret->f_id == id) {
			foo_hold(pret);
			break;
		}
	}
	pthread_mutex_unlock(&hashlock);
	return pret;
}

int foo_read(foo* pf) {
	int ret;

	pthread_mutex_lock(&pf->lock);
	ret = pf->f_value;
	pthread_mutex_unlock(&pf->lock);
	return ret;
}

void foo_update(foo* pf) {
	pthread_mutex_lock(&pf->lock);
	pf->f_value++;
	pthread_mutex_unlock(&pf->lock);
}

void foo_rele(foo* pf) {
	foo* find_header;
	unsigned long index;

	pthread_mutex_lock(&pf->lock);
	if (pf->f_count == 1) {
		pthread_mutex_unlock(&pf->lock);//先对foo结构中的lock解锁
		pthread_mutex_lock(&hashlock);	//再重新对从大锁到foo结构中的lock重新进行加锁
		pthread_mutex_lock(&pf->lock);
		if (pf->f_count != 1) {
			pf->f_count--;
			pthread_mutex_unlock(&hashlock);
			pthread_mutex_unlock(&pf->lock);
			return;
		}
		index = HASH(pf->f_id);			//从链表中删除节点
		find_header = foo_hash[index];
		if (find_header == pf)
			find_header = pf->pnext;
		else {
			while (find_header->pnext != pf)
				find_header = find_header->pnext;
			find_header->pnext = pf->pnext;
		}
		pthread_mutex_unlock(&pf->lock);//链表一旦修改完毕就应该将大锁进行释放
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_destroy(&pf->lock);
		free(pf);
		printf("foo freed\n");
	}
	else {
		pf->f_count--;
		pthread_mutex_unlock(&pf->lock);
	}
}

void* thread_func(void* args) {
	unsigned long id = (unsigned long)args;
	foo* pf = foo_find(id);
	for (int i = 0; i < 10000; i++)
		foo_update(pf);

	printf("At the of thread(%lu), pf->f_value = %d\n",
		id, foo_read(pf));
	foo_rele(pf);
	return (void*)NULL;
}
