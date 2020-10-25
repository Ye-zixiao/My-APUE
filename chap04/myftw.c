#include"apue.h"
#include<dirent.h>
#include<limits.h>

typedef int Myfunc(const char*, const struct stat*, int);

static Myfunc myfunc;
static int myftw(char*, Myfunc*);//一个自定义ftw函数指针
static int dopath(Myfunc*);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_quit("usage:ftw <starting-pathname>");
	
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

/*
	FTW_F：除了目录之外的文件
	FTW_D：目录
	FTW_DNR：不可读的目录
	FTW_NS：不可调用stat的文件
*/
enum {FTW_F,FTW_D,FTW_DNR,FTW_NS};

static char* fullpath;
static size_t pathlen;

static int myftw(char* pathname, Myfunc* func) {
	fullpath = path_alloc(&pathlen);

	/*让记录路径的数组逐步增长（按需增长）*/
	if (pathlen <= strlen(pathname)) {
		pathlen = strlen(pathname) * 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	strcpy(fullpath, pathname);

	//处理好路径字符串之后对这个开始路径下的子目录和文件进行遍历
	return dopath(func);
}

static int dopath(Myfunc* func) {
	struct stat statbuf;
	struct dirent* dirp;
	DIR* dp;
	int ret, n;

	if (lstat(fullpath, &statbuf) < 0)						//若当前目录不可stat，直接交给myfunc进行报错
		return func(fullpath, &statbuf, FTW_NS);
	if (S_ISDIR(statbuf.st_mode) == 0)						//若绝对路径是文件，此时没有必要向下递归遍历
		return func(fullpath, &statbuf, FTW_F);

	if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)		//若绝对路径指向的文件是目录，那么先将ndir++，然后向下递归遍历，这个判断语句其实没什么用
		return ret;

	n = strlen(fullpath);
	if (n + NAME_MAX + 2 > pathlen) {						//为开始目录下面的文件路径（必然比开始路径长）的字符数组空间重新分配
		pathlen *= 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	fullpath[n++] = '/';
	fullpath[n] = 0;

	if ((dp = opendir(fullpath)) == NULL)
		return func(fullpath, &statbuf, FTW_DNR);
	while ((dirp = readdir(dp)) != NULL) {
		if (strcmp(dirp->d_name, ".") == 0 ||
			strcmp(dirp->d_name, "..") == 0)
			continue;
		strcpy(&fullpath[n], dirp->d_name);					//将当前目录下的文件或者目录名字拼接到原来路径的后面,递归回来之后n是不变的，可以复用
		//printf("%s\n", fullpath);
		if ((ret = dopath(func)) != 0)
			break;
	}
	fullpath[n - 1] = 0;
	if (closedir(dp) < 0)
		err_ret("can't close directory %s", fullpath);
	return ret;
}


/*
* 对每一个file所需要执行的计数函数
*/
static int myfunc(const char* pathname, const struct stat* statptr, int type) {
	switch (type) {
	case FTW_F://若是非目录文件
		switch (statptr->st_mode & S_IFMT) {
		case S_IFREG:nreg++; break;
		case S_IFBLK:nblk++; break;
		case S_IFCHR:nchr++; break;
		case S_IFIFO:nfifo++; break;
		case S_IFLNK:nslink++; break;
		case S_IFSOCK:nsock++; break;
		case S_IFDIR:err_dump("for S_IFDIR for %s", pathname);
		}break;
	case FTW_D://若是文件
		ndir++; break;
	case FTW_DNR://若是不可读目录
		err_ret("can't read directory %s", pathname);
		break;
	case FTW_NS://若是不可调用stat函数的文件，或者说是上一个stat函数调用者调用失败
		err_ret("stat error for %s", pathname);
		break;
	default:
		err_dump("unknown type %d for pathname %s", type, pathname);
	}
	return 0;
}
