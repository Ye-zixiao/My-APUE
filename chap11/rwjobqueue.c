/*
*	提前说一些：
*		这种复杂且长的程序建议把相关的结构体定义、函数原型放到一个头文件见中，
*	而函数定义放到另一个源文件中，这样整个程序会比较清晰整洁，这里我出于整个
*	章节程序的整齐划一就不怎么做了！😊
* 
*	程序功能：
*		main主线程向工作队列放入用户指定个数或默认20的工作，然后创建的其他线程
*	从工作队列中取出属于自己的工作，然后及时的销毁并释放相关的资源。当这些工作
*	线程检查工作队列5次都没有任何属于自己的工作就终止该自身线程
* 
*/
#include"../include/MyAPUE.h"

//工作结构体
typedef struct job {
	struct job* pnext;
	struct job* pprev;
	pthread_t thread_id;
	int value;
}job;

//工作队列
typedef struct job_queue {
	job* head;
	job* tail;
	pthread_rwlock_t jq_lock;
}job_queue;


/*
	初始化工作队列
*/
int jobqueue_init(job_queue* jq) {
	int err;

	jq->head = NULL;
	jq->tail = NULL;
	if ((err = pthread_rwlock_init(&jq->jq_lock, NULL)) != 0)
		return err;
	return 0;
}

/*
	销毁工作队列
*/
void jobqueue_destroy(job_queue* jq) {
	if (jq == NULL)return;
	pthread_rwlock_destroy(&jq->jq_lock);
	free(jq);
}

/*
	前向插入工作
*/
void job_finsert(job_queue* jq, job* insertjob) {
	pthread_rwlock_wrlock(&jq->jq_lock);
	insertjob->pnext = jq->head;
	insertjob->pprev = NULL;
	if (jq->head == NULL)//若job_queue本身为空
		jq->tail = insertjob;
	else
		jq->head->pnext = insertjob;
	jq->head = insertjob;
	pthread_rwlock_unlock(&jq->jq_lock);
}

/*
	后向插入工作
*/
void job_binsert(job_queue* jq, job* insertjob) {
	pthread_rwlock_wrlock(&jq->jq_lock);
	insertjob->pprev = jq->tail;
	insertjob->pnext = NULL;
	if (jq->tail == NULL)//若job_queue本身为空
		jq->head = insertjob;
	else
		jq->tail->pnext = insertjob;
	jq->tail = insertjob;
	pthread_rwlock_unlock(&jq->jq_lock);
}

/*
	从队列中删除一个工作
*/
void job_remove(job_queue* jq, job* remove_job) {
	pthread_rwlock_wrlock(&jq->jq_lock);
	if (jq->head == remove_job) {
		jq->head = remove_job->pnext;
		if (jq->tail == remove_job)
			jq->tail = NULL;
		else
			remove_job->pnext->pprev = NULL;
	}
	else if (jq->tail == remove_job) {
		jq->tail = remove_job->pprev;
		if (remove_job == jq->head)
			jq->head = NULL;
		else
			remove_job->pprev->pnext = NULL;
	}
	else {
		remove_job->pnext->pprev = remove_job->pprev;
		remove_job->pprev->pnext = remove_job->pnext;
	}
	pthread_rwlock_unlock(&jq->jq_lock);
}

/*
	从工作队列中找到一个属于自己的工作
*/
job* job_find(job_queue* jq, pthread_t tid) {
	job* retp;

	if (pthread_rwlock_rdlock(&jq->jq_lock) != 0)
		return NULL;
	for (retp = jq->head; retp != NULL; retp = retp->pnext)
		if (pthread_equal(retp->thread_id, tid))
			break;
	pthread_rwlock_unlock(&jq->jq_lock);
	return retp;
}

/*
	返回工作队列长度
*/
int queue_size(job_queue* jq) {
	int size = 0;
	job* pjob;

	if (pthread_rwlock_rdlock(&jq->jq_lock) != 0)
		return -1;
	pjob = jq->head;
	while (pjob != NULL) {
		pjob = pjob->pnext;
		++size;
	}
	pthread_rwlock_unlock(&jq->jq_lock);

	return size;
}

/*
	初始化一个工作
*/
static job* job_init(job* pjob, pthread_t tid, int init_value) {
	pjob->thread_id = tid;
	pjob->value = init_value;
	return pjob;
}

/*
	线程函数，负责从工作队列中取出一个属于自己的工作并销毁它
*/
void* thread_func(void* args) {
	job_queue* jq = (job_queue*)args;
	job* myjob = NULL;
	int count = 5;
	char buf[64];

	while (1) {
		if ((myjob = job_find(jq, pthread_self())) != NULL) {
			count = 5;
			job_remove(jq, myjob);//从队列中取出属于自己的工作
			snprintf(buf, 64, "Thread(0x%lx) decrease value %d to 0\n",
				pthread_self(), myjob->value);
			while (myjob->value--);
			fputs(buf, stdout);
			free(myjob);
		}
		else {
			printf("Thread(0x%lx) doesn't find any job\n", pthread_self());
			if (--count == 0)//当5秒内没有发现新的工作就退出当前进程
				break;
		}
		sleep(1);
	}

	pthread_exit((void*)NULL);
}

/*
	------------------------------------------------
*/
int main(int argc,char*argv[])
{
	int count = 20;
	pthread_t tid1, tid2;
	job_queue* jq;
	job* pjob;

	if (argc > 1)
		count = atoi(argv[1]);

	if ((jq = malloc(sizeof(job_queue))) == NULL)
		err_sys("malloc error\n");
	if (jobqueue_init(jq) != 0)
		err_sys("jobqueue_init error\n");

	Pthread_create(&tid1, NULL, thread_func, (void*)jq);
	Pthread_create(&tid2, NULL, thread_func, (void*)jq);

	while (count--) {//主线程向工作队列中加入count*2个工作
		pjob = malloc(sizeof(job));
		job_binsert(jq, job_init(pjob, tid1, 100000000));
		pjob = malloc(sizeof(job));
		job_binsert(jq, job_init(pjob, tid2, 100000000));
	}
#ifdef DEBUG
	printf("job queue size = %d\n", queue_size(jq));
#endif
	Pthread_join(tid1, NULL);
	Pthread_join(tid2, NULL);
#ifdef DEBUG
	printf("job queue size = %d\n", queue_size(jq));
#endif
	jobqueue_destroy(jq);
	printf("process quit\n");

	exit(EXIT_SUCCESS);
}
