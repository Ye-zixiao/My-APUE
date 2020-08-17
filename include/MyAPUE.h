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
#include<time.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<pthread.h>

#define BUFSIZE 4096

/*
* -------------------------------------------------------------
*					Local Function Declaration
* -------------------------------------------------------------
*/

//错误例程
extern void err_sys(const char* msg);
extern void err_exit(int err, const char* msg);

//状态与信息
extern void pr_mask(const char* str);
extern void pr_exit(int status);
extern void pr_resuid(void);
extern void pr_now(void);

//信号传递
extern void TELL_WAIT(void);
extern void TELL_PARENT(pid_t pid);
extern void TELL_CHILD(pid_t pid);
extern void WAIT_PARENT(void);
extern void WAIT_CHILD(void);

//特殊系统函数
typedef void Sigfunc(int);
extern int System(const char* cmdstring);
extern int mysystem(const char* cmdstring);
extern void Abort(void);
extern unsigned int Sleep(unsigned int seconds);
extern Sigfunc* mysignal(int signo, Sigfunc*sighandler);

//自定义线程函数
extern void Pthread_create(pthread_t* tid,
	const pthread_attr_t* attr, void* (*thread_func)(void*), void* args);
extern void Pthread_join(pthread_t tid, void** rval_ptr);

#endif
