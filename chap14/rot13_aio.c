#include "../include/MyAPUE.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <aio.h>

#define NBUF 8

//异步I/O缓冲区状态
enum AioStat {
	UNUSED,
	READ_PENDING,
	WRITE_PENDING
};


//异步I/O缓冲区
typedef struct AIOBuf {
	unsigned char data[BUFSIZE];
	struct aiocb aiocb_buf;
	enum AioStat status;
	int last;
}AIOBuf;

AIOBuf bufs[NBUF];


unsigned char 
translate(unsigned char ch) {
	if (isalpha(ch)) {
		if (ch >= 'n')
			ch -= 13;
		else if (ch < 'n')
			ch += 13;
		else if (ch >= 'N')
			ch -= 13;
		else
			ch += 13;
	}
	return(ch);
}


int main(int argc, char* argv[])
{
	const struct aiocb* aiolist[NBUF];
	int numop, err, retval;
	struct stat statbuf;
	int ifd, ofd;
	off_t off = 0;

	if (argc != 3)
		err_quit("usage: %s infile outfile", argv[0]);
	if ((ifd = open(argv[1], O_RDONLY)) < 0)
		err_sys("can't open file %s", argv[1]);
	if ((ofd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, FILE_MODE)) < 0)
		err_sys("can't create file %s", argv[2]);
	if (fstat(ifd, &statbuf) < 0)
		err_sys("fstat error");

	//初始化异步I/O缓冲区中的异步I/O控制块和异步I/O列表
	for (int i = 0; i < NBUF; ++i) {
		bufs[i].last = 0;
		bufs[i].status = UNUSED;
		bufs[i].aiocb_buf.aio_buf = bufs[i].data;
		bufs[i].aiocb_buf.aio_sigevent.sigev_notify = SIGEV_NONE;
		aiolist[i] = NULL;

	}

	/*
	 * 每一轮让主进程并不停止：
	   1.若遇到状态为UNUSED的异步I/O缓冲区，则向其分配从输入文件中异步读取数据的任务
	   2.若遇到状态为READ_PENDING的异步I/O缓冲区，则检测是否读取完毕。若无跳过，否则
	     向其分配一个数据处理+异步写入到输出文件的任务
	   3.若遇到一个WRITE_PENDING的异步I/O缓冲区，则检测是否写入完毕。若无跳过，否则
	     向其重新将该缓冲区置为UNUSED状态
	   当主进程完成一轮的事务后发现总任务还没有完成，不管是否是否真的完成异步读/写的工作，
	   都会调用aio_suspend函数等待上一轮中分配的异步I/O先完成再开始下一轮（否则进程无法
	   继续下一轮的分配异步I/O的事务）
	 */
	numop = 0;
	while (1) {
		for (int i = 0; i < NBUF; ++i) {
			switch (bufs[i].status) {
			case UNUSED:
				if (off <= statbuf.st_size) {
					bufs[i].status = READ_PENDING;
					bufs[i].aiocb_buf.aio_fildes = ifd;
					bufs[i].aiocb_buf.aio_offset = off;
					bufs[i].aiocb_buf.aio_nbytes = BUFSIZE;
					off += BUFSIZE;
					if (off >= statbuf.st_size)
						bufs[i].last = 1;

					//发出一个异步读取的请求给操作系统
					if (aio_read(&bufs[i].aiocb_buf) < 0)
						err_sys("aio_read error");
					aiolist[i] = &bufs[i].aiocb_buf;
					numop++;
				}
				break;
			case READ_PENDING:
				//检测是否完成异步读取的任务
				if ((err = aio_error(&bufs[i].aiocb_buf)) == EINPROGRESS)
					continue;
				if (err != 0) {
					if (err == -1)
						err_sys("aio_error error");
					err_exit(err, "read error");
				}

				if ((retval = aio_return(&bufs[i].aiocb_buf)) < 0)
					err_sys("aio_return error");
				if (retval != BUFSIZE && !bufs[i].last)
					err_sys("short read (%d/%d)", retval, BUFSIZE);

				//翻译字符
				for (int j = 0; j < BUFSIZE; ++j)
					bufs[i].data[j] = translate(bufs[i].data[j]);

				//开启一个异步写入的请求给操作系统
				bufs[i].status = WRITE_PENDING;
				bufs[i].aiocb_buf.aio_fildes = ofd;
				bufs[i].aiocb_buf.aio_nbytes = retval;
				if (aio_write(&bufs[i].aiocb_buf) < 0)
					err_sys("aio_write error");
				break;
			case WRITE_PENDING:
				//检测异步写入是否完成
				if ((err = aio_error(&bufs[i].aiocb_buf)) == EINPROGRESS)
					continue;
				if (err != 0) {
					if (err == -1)
						err_sys("aio_error error");
					err_exit(err, "write error");
				}

				if ((retval = aio_return(&bufs[i].aiocb_buf)) < 0)
					err_sys("aio_return error");
				if (retval != bufs[i].aiocb_buf.aio_nbytes)
					err_quit("short write (%d/%d)", retval, bufs[i].aiocb_buf.aio_nbytes);

				//将异步I/O缓冲区状态转换回UNUSED状态
				bufs[i].status = UNUSED;
				aiolist[i] = NULL;
				numop--;
				break;
			}
		}
		if (numop == 0) {
			if (off >= statbuf.st_size)
				break;
		}
		else {
			//等待上一轮中的异步I/O操作完成，以使得后续的异步操作可以得到正常的分配
			if (aio_suspend(aiolist, NBUF, NULL) < 0)
				err_sys("aio_suspend error");
		}
	}

	//要求暂存在内核中的写入数据强制同步写入到硬盘中
	bufs[0].aiocb_buf.aio_fildes = ofd;
	if (aio_fsync(O_SYNC, &bufs[0].aiocb_buf) < 0)
		err_sys("aio_fsync error");
	exit(EXIT_SUCCESS);
}


/*
 * 在实践中，让一个阻塞I/O执行由createfile.py创建的文件的翻译工作，
 * 总共花费了0.020s；而让一个异步I/O执行由createfile.py创建的文件
 * 的翻译工作却总共花费了0.072s的时间！即使让文件变得很大，这个异步
 * I/O程序仍然不能让文件的翻译工作变得更快，反而比阻塞I/O更慢！
 * */
