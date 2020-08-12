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

#define BUFSIZE 4096

/*
* -------------------------------------------------------------
*					Local Function Declaration
* -------------------------------------------------------------
*/

//状态与信息
extern void err_sys(const char* msg);

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
extern int System(const char* cmdstring);
extern int mysystem(const char* cmdstring);
extern void Abort(void);
extern unsigned int Sleep(unsigned int seconds);

#endif