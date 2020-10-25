#include"MyAPUE.h"

int main(void)
{
	printf("pid  : %d\n", getpid());
	printf("ppid : %d\n", getppid());
	printf("uid  : %d\n", getuid());
	printf("gid  : %d\n", getgid());
	printf("euid : %d\n", geteuid());
	printf("egid : %d\n", getegid());

	return 0;
}