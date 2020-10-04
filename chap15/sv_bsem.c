#include "../include/MyAPUE.h"
#include <sys/mman.h>
#include <string.h>

#define DEFAULT_CNT 10000

struct SharedVal {
	void* pmm;
	lock_t lock;
} svalue;


static void pro_do(struct SharedVal*psv) {
	int temp;
	char buf[BUFSIZE];

	for (int i = 0; i < DEFAULT_CNT; ++i) {
		if (SvSemLock_Lock(&psv->lock) == -1)
			err_sys("lock error");
		
		strcpy(buf, psv->pmm);
		temp = atoi(buf);
		sprintf(buf, "%d\n", ++temp);
		strcpy(psv->pmm, buf);

		if (SvSemLock_Unlock(&psv->lock) == -1)
			err_sys("unlock error");
	}
}


int main(int argc, char* argv[])
{
	int fd;
	pid_t pid;

	if (argc != 2)
		err_sys("usage: %s <filename>", argv[0]);

	//创建或者打开文件
	if ((fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, FILE_MODE)) < 0)
		err_sys("open error");
	if (ftruncate(fd, 64) == -1)
		err_sys("ftruncate error");
	if (write(fd, "0", 2) != 2)
		err_sys("write error");

	//初始化System V二值信号量
	if (SvSemLock_Init(&svalue.lock) == -1)
		err_sys("lock init error");
	if ((svalue.pmm = mmap(0, BUFSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0))
		== MAP_FAILED)
		err_sys("mmap error");

	//创建父子进程共同递增映射存储区中的数值
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		pro_do(&svalue);
		_exit(EXIT_SUCCESS);
	}
	else {
		pro_do(&svalue);
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error");
		printf("svalue final result: %d\n", atoi(svalue.pmm));
		if (msync(svalue.pmm, BUFSIZE, MS_SYNC) == -1)
			err_sys("msync error");
		if (munmap(svalue.pmm, BUFSIZE) == -1)
			err_sys("munmap error");
		if (SvSemLock_Destroy(&svalue.lock) == -1)
			err_sys("lock destroy error");
	}
	
	exit(EXIT_SUCCESS);
}
