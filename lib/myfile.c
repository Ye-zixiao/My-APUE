#include "../include/MyAPUE.h"


/**
 * 根据stat结构体的文件模式成员返回文件类型字符串
 */
const char* file_type1(__mode_t mode,int simple) {
	const char* ret = NULL;

	if (S_ISREG(mode))
		ret = simple ? "-" : "regular";
	else if (S_ISDIR(mode))
		ret = simple ? "d" : "directory";
	else if (S_ISCHR(mode))
		ret = simple ? "c" : "character";
	else if (S_ISBLK(mode))
		ret = simple ? "b" : "block";
	else if (S_ISFIFO(mode))
		ret = simple ? "p" : "FIFO";
	else if (S_ISLNK(mode))
		ret = simple ? "l" : "symbolic link";
	else if (S_ISSOCK(mode))
		ret = simple ? "s" : "socket";
	else
		ret = simple ? "?" : "unrecognized file type";
	return ret;
}


/**
 * 根据stat结构体的文件模式成员返回文件权限字符串
 */
const char* file_mode(__mode_t mode) {
	static char modestr[10];

	modestr[0] = (mode & S_IRUSR) ? 'r' : '-';
	modestr[1] = (mode & S_IWUSR) ? 'w' : '-';
	modestr[2] = (mode & S_IXUSR) ? 'x' : '-';
	modestr[3] = (mode & S_IRGRP) ? 'r' : '-';
	modestr[3] = (mode & S_ISUID) ? 's' : modestr[3];
	modestr[4] = (mode & S_IWGRP) ? 'w' : '-';
	modestr[5] = (mode & S_IXGRP) ? 'x' : '-';
	modestr[5] = (mode & S_ISGID) ? 'g' : modestr[5];
	modestr[6] = (mode & S_IROTH) ? 'r' : '-';
	modestr[7] = (mode & S_IWOTH) ? 'w' : '-';
	modestr[8] = (mode & S_IXOTH) ? 'x' : '-';
	modestr[8] = (mode & S_ISVTX) ? 't' : modestr[8];
	modestr[9] = '\0';
	return modestr;
}
