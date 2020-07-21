#include<stdio.h>
#include<dirent.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
	if (argc != 2)
		err_sys("usage:mydir [dirname]\n");

	DIR* dp;
	int fd;
	struct dirent* dirp;

	if ((fd = open(argv[1], O_RDONLY)) < 0)
		err_sys("open error\n");
	if ((dp = fdopendir(fd)) == NULL)
		err_sys("fdopendir error\n");
	while ((dirp = readdir(dp)) != NULL)
		printf("%s\n", dirp->d_name);

	closedir(dp);
	close(fd);
	exit(EXIT_SUCCESS);
}
