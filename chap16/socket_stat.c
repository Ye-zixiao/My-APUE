#include "../include/MyAPUE.h"


int main(void)
{
	struct stat statbuf;
	int sockfd;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");

	if (fstat(sockfd, &statbuf) == -1)
		err_sys("fstat error");
	printf("file type     : %s\n", file_type(statbuf.st_mode));
	printf("inode number  : %lu\n", statbuf.st_ino);
	printf("link number   : %lu\n", statbuf.st_nlink);
	printf("file mode     : %s\n", file_mode(statbuf.st_mode));
	printf("uid           : %u\n", statbuf.st_uid);
	printf("gid           : %u\n", statbuf.st_gid);
	printf("file size     : %ld\n", statbuf.st_size);
	printf("atime         : %s\n", transformTime(statbuf.st_atim.tv_sec,
		                                         "%m-%d %T"));

	close(sockfd);
	exit(EXIT_SUCCESS);
}
