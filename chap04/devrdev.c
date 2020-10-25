#include"apue.h"
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/sysmacros.h>

int main(int argc, char** argv)
{
	int i;
	struct stat statbuf;
	for (i = 1; i < argc; ++i) {
		printf("%s: ", argv[i]);
		if (stat(argv[i], &statbuf) < 0) {
			err_ret("stat error");
			continue;
		}
		printf("dev=%d/%d", major(statbuf.st_dev), minor(statbuf.st_dev));

		if (S_ISBLK(statbuf.st_mode) || S_ISCHR(statbuf.st_mode))//若是特殊设备（块设备或特殊字符设备）
			printf("(%s) rdev=%d/%d", (S_ISCHR(statbuf.st_mode)) ? "character" : "block",
				major(statbuf.st_rdev), minor(statbuf.st_rdev));

		putchar('\n');
	}
	exit(EXIT_SUCCESS);
}
