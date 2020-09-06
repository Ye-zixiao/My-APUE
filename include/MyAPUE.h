#ifndef _MY_AUPE_H_
#define _MY_APUE_H_

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#ifndef _GNU_SOURCE
#define	_GNU_SOURCE
#endif

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<time.h>
#include<pthread.h>

#define BUFSIZE 4096
#define SBUFSIZE 64

/*
* -------------------------------------------------------------
*                 自定义函数原型
* -------------------------------------------------------------
*/


//错误例程
void err_sys(const char* msg);
void err_exit(int err, const char* msg);


//状态与信息
void pr_mask(const char* str);
void pr_exit(int status);
void pr_resuid(void);
void pr_now(void);
void _pr_limit(const char* rname, int resource);
void get_abstime(struct timespec* tsp, long seconds);
int pr_mutexattr(const pthread_mutexattr_t* mutexattr);

#define pr_limit(resource) _pr_limit(#resource,resource)

char* mygetenv(const char* name);
int mygetenv_r(const char* name, char* buf, int buflen);


//信号传递
void TELL_WAIT(void);
void TELL_PARENT(pid_t pid);
void TELL_CHILD(pid_t pid);
void WAIT_PARENT(void);
void WAIT_CHILD(void);


//特殊系统函数
typedef void Sigfunc(int);
int System(const char* cmdstring);
int mysystem(const char* cmdstring);
void Abort(void);
unsigned int Sleep(unsigned int seconds);
Sigfunc* mysignal(int signo, Sigfunc*sighandler);


//自定义线程函数
void Pthread_create(pthread_t* tid,
	const pthread_attr_t* attr, void* (*thread_func)(void*), void* args);
void Pthread_join(pthread_t tid, void** rval_ptr);
int makeDetachedThread(void* (*thread_func)(void*), void* args);


//算法例程
int greater(const int*, const int*);
int shorter(const int*, const int*);
void voilent_sort(int arr[], int n, int(*pf)(const int*, const int*));
void select_sort(int arr[], int n, int(*pf)(const int*, const int*));
void insert_sort(int arr[], int n, int(*pf)(const int*, const int*));
void heap_sort(int arr[], int n, int(*pf)(const int*, const int*));
void bubble_sort(int arr[], int n, int(*pf)(const int*, const int*));


#endif