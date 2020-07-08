#include"apue.h"
#include<dirent.h>

int main(int argc, char** argv)
{
	DIR* pd;
	struct dirent* pdir;
	if (argc != 2)
		err_quit("usage:ls dir_name");
	if((pd=opendir(argv[1]))==NULL)
		err_sys("can't open %s",argv[1]);

	while ((pdir = readdir(pd)) != NULL)
		printf("%s\n", pdir->d_name);

	closedir(pd);
	exit(0);
}