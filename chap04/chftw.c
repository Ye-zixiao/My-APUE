#include"apue.h"
#include<dirent.h>
#include<limits.h>

/*
* 1、myftw:为指定路径下的所有文件执行func操作
* 2、dopath:本质上为myftw实质性工作
* 3、myfunc:作用于文件之上的操作
*/

typedef int Myfunc(const char* filename, const struct stat* pstat, int flags);
static Myfunc myfunc;
static int dopath(const char* filename, Myfunc* func);
static int myftw(const char* pathname, Myfunc* func);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;
enum{FTW_F,FTW_D,FTW_DNR,FTW_DNCHD,FTW_NS};
//文件、目录、目录不可读、目录不可切换、目录不可stat


int main(int argc, char* argv[])
{
	if (argc != 2)
		err_quit("usage:myftw <starting-pathname>");

	int ret = myftw(argv[1], myfunc);
	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	if (ntot == 0)
		ntot = 1;

	printf("regular files  = %7ld, %5.2f %%\n", nreg, nreg * 100.0 / ntot);
	printf("directories    = %7ld, %5.2f %%\n", ndir, ndir * 100.0 / ntot);
	printf("block special  = %7ld, %5.2f %%\n", nblk, nblk * 100.0 / ntot);
	printf("char special   = %7ld, %5.2f %%\n", nchr, nchr * 100.0 / ntot);
	printf("FIFOs          = %7ld, %5.2f %%\n", nfifo, nfifo * 100.0 / ntot);
	printf("symbolic links = %7ld, %5.2f %%\n", nslink, nslink * 100.0 / ntot);
	printf("sockets        = %7ld, %5.2f %%\n", nsock, nsock * 100.0 / ntot);

	exit(ret);
}


/*----------------------------------------------------------------------------*/

static int 
myftw(const char* pathname, Myfunc* func) {
	return dopath(pathname, func);
}

static int 
dopath(const char* filename, Myfunc* func) {
	struct stat statbuf;
	struct dirent* pentry;//指向目录项entry
	DIR* pdir;
	int ret;

	if (lstat(filename, &statbuf) < 0)
		return func(filename, &statbuf, FTW_NS);
	if (S_ISDIR(statbuf.st_mode) == 0)
		return func(filename, &statbuf, FTW_F);
	
	//下面处理目录:
	if ((ret = func(filename, &statbuf, FTW_D)) != 0)
		return ret;
	if (chdir(filename) < 0)
		return func(filename, &statbuf, FTW_DNCHD);
	if ((pdir = opendir(".")) == NULL)
		return func(filename, &statbuf, FTW_DNR);
	while ((pentry = readdir(pdir)) != NULL) {
		if (strcmp(pentry->d_name, ".") == 0 ||
			strcmp(pentry->d_name, "..") == 0)
			continue;
		if ((ret = dopath(pentry->d_name, func)) != 0)
			break;
	}
	
	if (closedir(pdir) < 0 || chdir("..") < 0)
		err_sys("closedir error or chdir error");
	return ret;
}

static int 
myfunc(const char* filename, const struct stat* pstat, int flags) {
	switch (flags) {
	case FTW_F:
		switch (pstat->st_mode & S_IFMT) {
		case S_IFREG:++nreg; break;
		case S_IFBLK:++nblk; break;
		case S_IFCHR:++nchr; break;
		case S_IFIFO:++nfifo; break;
		case S_IFLNK:++nslink; break;
		case S_IFSOCK:++nsock; break;
		default:err_ret("unkown type %s", filename);
		}break;
	case FTW_D:
		++ndir; break;
	case FTW_DNCHD:
		err_ret("chdir %s error", filename); break;
	case FTW_DNR:
		err_ret("read diretory %s error", filename); break;
	case FTW_NS:
		err_ret("stat %s error", filename); break;
	default:
		err_dump("unkown type");
	}
	return 0;
}