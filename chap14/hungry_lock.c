/**
 * 检测当前系统中是否可以发生加写锁进程被饿死
 */
#include "../include/MyAPUE.h"
#include <fcntl.h>

#define DEFAULT_FILEPATH "/tmp/testfile"

void rwPStat(const char* target, const char* stat) {
	printf("%12s proccess(%lx) %5s, %s\n",
		target, (long)getpid(), stat, currTime());
}

int main(void)
{
	int fd;
	pid_t pid1, pid2;

	if ((fd = open(DEFAULT_FILEPATH, O_CREAT | O_RDWR | O_TRUNC, FILE_MODE)) < 0)
		err_sys("open error");
	/* 父进程在指定文件上获取记录读锁（以非阻塞方式），大约持续3秒 */
	rwPStat("read lock", "start");
	if (read_lock(fd, 0, SEEK_SET, 0) == -1)
		err_sys("read_lock error");
	rwPStat("read lock", "locks");

	/* 创建第一个子进程，并让该子进程试图对指定文件获取记录写锁
		（以阻塞方式），同样持续3秒 */
	if ((pid1 = fork()) < 0)
		err_sys("fork error");
	else if (pid1 == 0) {
		rwPStat("write lock", "start");
		if (writew_lock(fd, 0, SEEK_SET, 0) == -1)
			err_sys("writew_lock error");
		rwPStat("write lock", "locks");

		sleep(3);
		exit(EXIT_SUCCESS);
	}

	/* 创建第二个子进程，并让该子进程试图对指定文件获取记录读锁
		（以阻塞方式）同样持续3秒 */
	if ((pid2 = fork()) < 0)
		err_sys("fork error");
	else if (pid2 == 0) {
		rwPStat("read lock", "start");
		if (readw_lock(fd, 0, SEEK_SET, 0) == -1)
			err_sys("readw_lock error");
		rwPStat("read lock", "locks");

		sleep(3);
		exit(EXIT_SUCCESS);
	}

	sleep(3);
	exit(EXIT_SUCCESS);
}

/*
实验结果：
$ ./main
   read lock proccess(d3e) start, 15:36:36
   read lock proccess(d3e) locks, 15:36:36
  write lock proccess(d3f) start, 15:36:36
   read lock proccess(d40) start, 15:36:36
   read lock proccess(d40) locks, 15:36:36
  write lock proccess(d3f) locks, 15:36:39

  可以看到我们以读-写-读的获取记录锁的方式创建了3个进程，虽然试图获取写记录锁的
  进程先于第三个试图获取记录读锁的进程创建，但是该试图获取记录写锁的进程只有在
  所有的记录读锁进程释放了记录读锁之后才得到机会并获取到自己的记录写锁。
	因此可以推断在本人的系统上（没有经过任何特殊的设置）存在试图加写锁的进程
  被饿死的现象

*/
