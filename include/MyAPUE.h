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
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <stddef.h>

#define BUFSIZE 4096
#define SBUFSIZE 64

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define DIR_MODE (FILE_MODE|S_IXUSR|S_IXGRP|S_IXOTH)
#define IPC_PERM 0666


/**
 * 自定义调试函数
 */
void debug(void);


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
 * 文件I/O
 */
const char* file_mode(__mode_t mode);
const char* file_type1(__mode_t mode,int simple);
#define file_type(mode)	\
		file_type1((mode),0)
void pr_mask(const char* str);


/**
 * 时间
 */
const char* currTime(const char*format);
const char* transformTime(time_t t, const char* format);
void get_abstime(struct timespec* tsp, long seconds);

#define pr_now() \
	printf("Current time: %s\n",currTime(NULL))


/**
 * 输出状态与信息
 */
void pr_exit(int status);
void pr_resuid(void);
void _pr_limit(const char* rname, int resource);
int pr_mutexattr(const pthread_mutexattr_t* mutexattr);

#define pr_limit(resource) _pr_limit(#resource,resource)

char* mygetenv(const char* name);
int mygetenv_r(const char* name, char* buf, int buflen);
char* getenv_r(const char* name);
long open_max(void);

const char* getusername(uid_t uid);
const char* getgrpname(gid_t gid);


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
Sigfunc* mysignal(int signo, Sigfunc* sighandler);


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
typedef int _Comp(const int*, const int*);
_Comp greater;
_Comp shorter;
void BFSort(int arr[], int n, _Comp* pf);
void BubbleSort(int arr[], int n, _Comp* pf);
void SelectSort(int arr[], int n, _Comp* pf);
void InsertionSort(int arr[], int n, _Comp* pf);
void ShellSort(int arr[], int n, _Comp* pf);
void HeapSort(int arr[], int n, _Comp* pf);
void mergeArr(int arr[], int, int, int, _Comp* pf);
void MergeSort(int arr[], int n, _Comp* pf);
void QuitSort(int arr[], int n, _Comp* pf);


/**
 * 守护进程例程
 */
void daemonize(const char* str);
int singleDaemon(const char* file);


/**
 * 高级I/O函数
 */
int set_fd(int, int);
int set_fl(int, int);
int clr_fd(int, int);
int clr_fl(int, int);
int set_cloexec(int fd);
int lock_reg(int, int, int, off_t, int, int);
int lock_test(int, int, int, off_t, int, int);

//常规记录锁操作
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

#define lockfile(fd) write_lock((fd), 0, SEEK_SET, 0)

//是否可设置读锁
#define is_read_lockable(fd, offset, whence, len) \
	(lock_test((fd), F_GETLK, F_RDLCK, (offset), (whence), (len)) == 0)
//是否可设置写锁
#define is_write_lockable(fd, offset, whence, len) \
	(lock_test((fd), F_GETLK, F_WRLCK, (offset), (whence), (len)) == 0)

ssize_t readn(int fd, void* ptr, size_t nbytes);
ssize_t writen(int fd, const  void* ptr, size_t nbytes);


/**
 * 进程间通信
 */
FILE* Popen(const char* cmdstring, const char* type);
int Pclose(FILE* pf);

//使用System V(XSI)信号量实现的二值信号量
typedef int lock_t;
int BSem_Create(lock_t* lock, const char* path, int projid);
int BSem_Create1(lock_t* lock, const char* path, int projid);
int BSem_Init(lock_t* lock, int initv);
int BSem_Destroy(lock_t* lock);
int BSem_Sub(lock_t* lock);
int BSem_Add(lock_t* lock);

//使用POSIX信号量封装出的二值信号量
#define PBSem_Init(lock, pshared) \
		sem_init((lock), (pshared), 1)
#define PBSem_Destroy(lock) \
		sem_destroy(lock)
#define PBSem_Lock(lock)	 \
		sem_wait(lock)
#define PBSem_Trylock(lock) \
		sem_trywait(lock)
#define PBSem_Timedwait(lock, tsptr)	 \
		sem_timedwait((lock), (tsptr))
#define PBSem_Unlock(lock) \
		sem_post((lock))


/**
 * socket编程
 */
int connect_retry(int domain, int type, int protocol,
				const struct sockaddr* addr, socklen_t alen);
int init_server(int type, const struct sockaddr* addr,
				socklen_t alen, int qlen);

int serv_listen(const char* name);
int serv_accept(int listenfd, uid_t* uidptr);
int cli_conn(const char* name);

//提供类似于serv_listen类似的功能但是可以支持backlog选项
int server_listen(const char* name, int qlen);

#define fd_pipe(fds)		\
		socketpair(AF_UNIX,SOCK_STREAM,0,(fds))

/**
 * 传送文件描述符
 */
int send_fd(int sockfd, int fd_to_send);
int send_err(int sockfd, int status, const char* errmsg);
int recv_fd(int sockfd, ssize_t(*userfunc)(int, const void*, size_t));

int send_ufd(int sockfd, int fd_to_send);
int recv_ufd(int sockfd, uid_t* uidptr, ssize_t(*userfunc)(int, const void*, size_t));


#endif // !MY_AUPE_H_