#include "../include/MyAPUE.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define COPYINCR (1024*1024*1024)

int main(int argc, char* argv[])
{
	off_t fsz = 0;
	int ifd, ofd;
	size_t copysize;
	void* src, * dest;
	struct stat statbuf;

	//打开输入文件并创建输出文件，设置其长度
	if (argc != 3)
		err_quit("usage: %s <srcfile> <targetfile>", argv[0]);
	if ((ifd = open(argv[1], O_RDONLY)) < 0)
		err_sys("can't open infile %s for reading", argv[1]);
	if ((ofd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0)
		err_sys("can't open outfile %d for writing", argv[2]);
	if (fstat(ifd, &statbuf) < 0)
		err_sys("fstat error");
	if (ftruncate(ofd, statbuf.st_size) < 0)
		err_sys("ftruncate error");

	while (fsz < statbuf.st_size) {
		if ((statbuf.st_size - fsz) > COPYINCR)
			copysize = COPYINCR;
		else
			copysize = statbuf.st_size - fsz;
#ifdef DEBUG
		printf("copysize = %lu\n", copysize);
#endif

		//分配存储映射区，将磁盘文件映射到缓冲区中
		if ((src = mmap(0, copysize, PROT_READ, MAP_SHARED, ifd, fsz))
			== MAP_FAILED)
			err_sys("mmap error for input");
		if ((dest = mmap(0, copysize, PROT_WRITE | PROT_READ, MAP_SHARED, ofd, fsz))
			== MAP_FAILED)
			err_sys("mmap error for output");
#ifdef DEBUG
		printf("dest = %p, dest mod PAGESIZE = %ld\n", dest, (long)dest % getpagesize());
#endif
		//从输入文件存储映射区拷贝数据到输出文件存储映射区
		memcpy(dest, src, copysize);
		//msync(dest,copysize,MS_SYNC);
		munmap(src, copysize);
		munmap(dest, copysize);
		fsz += copysize;
	}

	exit(EXIT_SUCCESS);
}
