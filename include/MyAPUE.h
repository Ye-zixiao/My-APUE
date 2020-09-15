#ifndef MY_AUPE_H_
#define MY_APUE_H_

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#ifndef _GNU_SOURCE
#define	_GNU_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

#define BUFSIZE 4096
#define SBUFSIZE 64

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define DIR_MODE (FILE_MODE|S_IXUSR|S_IXGRP|S_IXOTH)


/**
 * 普通程序出错例程 
 */
void err_ret(const char* fmt, ...);
void err_msg(const char* fmt, ...);
void err_cont(int err, const char* fmt, ...);
void err_sys(const char* fmt, ...);
void err_exit(int err, const char* fmt, ...);
void err_dump(const char* fmt, ...);
void err_quit(const char* fmt, ...);


/**
 * 守护进程出错例程
 */
void log_open(const char* ident, int option, int facility);
void log_ret(const char* fmt, ...);
void log_msg(const char* fmt, ...);
void log_sys(const char* fmt, ...);
void log_quit(const char* fmt, ...);
void log_exit(int error, const char* fmt, ...);


/**
 * 输出状态与信息
 */
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
char* getenv_r(const char* name);


/**
 * 信号传递
 */
void TELL_WAIT(void);
void TELL_PARENT(pid_t pid);
void TELL_CHILD(pid_t pid);
void WAIT_PARENT(void);
void WAIT_CHILD(void);


/**
 * 特殊系统函数
 */
typedef void Sigfunc(int);
int System(const char* cmdstring);
int mysystem(const char* cmdstring);
void Abort(void);
unsigned int Sleep(unsigned int seconds);
Sigfunc* mysignal(int signo, Sigfunc*sighandler);


/**
 * 自定义线程函数
 */
void Pthread_create(pthread_t* tid,
	const pthread_attr_t* attr, void* (*thread_func)(void*), void* args);
void Pthread_join(pthread_t tid, void** rval_ptr);
int makeDetachedThread(void* (*thread_func)(void*), void* args);


/**
 * 算法例程
 */
int greater(const int*, const int*);
int shorter(const int*, const int*);
void BFSort(int arr[], int n, int(*)(const int*, const int*));
void SelectSort(int arr[], int n, int(*)(const int*, const int*));
void InsertionSort(int arr[], int n, int(*)(const int*, const int*));
void HeapSort(int arr[], int n, int(*)(const int*, const int*));
void BubbleSort(int arr[], int n, int(*)(const int*, const int*));
void mergeArr(int arr[], int, int, int, int(*)(const int*, const int*));
void MergeSort(int arr[], int n, int(*)(const int*, const int*));


/**
 * 守护进程例程
 */
void daemonize(const char* str);
int singleDaemon(const char* file);


/**
 * 高级I/O函数
 */
int lockfile(int);
int set_fd(int, int);
int set_fl(int, int);
int clr_fd(int, int);
int clr_fl(int, int);
int lock_reg(int, int, int, off_t, int, int);
int lock_test(int, int, int, off_t, int, int);

/**
 * 对文件进行加锁
 */
#define read_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

/**
 * 测试文件记录锁是否被加锁
 */
//是否可请求读锁
#define is_read_lockable(fd, offset, whence, len) \
	(lock_test((fd), F_GETLK, F_RDLCK, (offset), (whence), (len)) == 0)
//是否可请求写锁
#define is_write_lockable(fd, offset, whence, len) \
	(lock_test((fd), F_GETLK, F_WRLCK, (offset), (whence), (len)) == 0)



#endif // !MY_AUPE_H_