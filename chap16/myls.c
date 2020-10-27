/**
 * 实现一个简单的ls目录文件例举程序 
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <dirent.h>

#define SIZESTEP 1024

/**
 * 将文件字节大小转换为人类易读的表示形式
 */
const char* human_readable(double size, int rflag) {
	static char buf[64];

	if (rflag == 0 || size < SIZESTEP)
		sprintf(buf, "%.f", size);
	else if ((size /= SIZESTEP) < SIZESTEP)
		sprintf(buf, "%.1fK", size);
	else if ((size /= SIZESTEP) < SIZESTEP)
		sprintf(buf, "%.1fM", size);
	else if ((size /= SIZESTEP) < SIZESTEP)
		sprintf(buf, "%.1fG", size);
	else if ((size / SIZESTEP) < SIZESTEP)
		sprintf(buf, "%.1fT", size);
	return buf;
}


int main(int argc, char* argv[])
{
	const char* dirname = ".";
	int ch, l, i, r, a;
	struct stat statbuf;
	struct dirent* pent;
	DIR* pdir;

	opterr = 0;
	l = i = r = a = 0;
	while ((ch = getopt(argc, argv, "hairl")) != -1) {
		switch (ch) {
		case 'h':
			err_quit("usage: myls [-l] [-r] [dirname]\n"
					"       myls [-h]\n"
				    "a: list all\n"
					"h: for help\n"
					"l: for detail information\n"
					"r: make size human readable");
		case 'a':
			a = 1; break;
		case 'i':
			i = 1; break;
		case 'r':
			r = 1; break;
		case 'l':
			l = 1; break;
		case '?':
			err_quit("unrecognized option: -%c", optopt);
		}
	}
	/* 若提供了目录名，则将欲遍历目录进行修改 */
	if (optind <= argc - 1)
		dirname = argv[optind];

	if ((pdir = opendir(dirname)) == NULL)
		err_sys("opendir error");
	if (chdir(dirname) == -1)
		err_sys("chdir error");
	while ((pent = readdir(pdir)) != NULL) {
		if (!a && pent->d_name[0] == '.')
			continue;

		if (lstat(pent->d_name, &statbuf) == -1)
			err_sys("stat error");
		//inode
		i ? printf("%-8lu ", statbuf.st_ino) : 0;
		//detail
		l ? printf("%s%s %3lu %s %s %6s %s ", file_type1(statbuf.st_mode, 1),
			file_mode(statbuf.st_mode), statbuf.st_nlink, getusername(statbuf.st_uid),
			getgrpname(statbuf.st_gid), human_readable((double)statbuf.st_size, r),
			transformTime(statbuf.st_atim.tv_sec, "%m-%d %T")) : 0;
		printf("%s\n", pent->d_name);
	}
	if (closedir(pdir) == -1)
		err_sys("closedir error");

	exit(EXIT_SUCCESS);
}
