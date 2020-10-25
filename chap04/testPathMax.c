#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<limits.h>
#include<string.h>

#define DEFAULT_MODE (S_IRUSR|S_IWUSR|S_IXUSR)

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	long pathMax = pathconf("/", _PC_PATH_MAX);
	printf("pathMax=%ld\n", pathMax);
	pathMax *= 3;
	char pathchar[pathMax];
	
	while (1) {
		if (mkdir("test", DEFAULT_MODE) < 0)
			err_sys("mkdir error\n");
		if (chdir("test") < 0)
			err_sys("chdir error\n");

		if ((getcwd(pathchar, pathMax)) != NULL)
			printf("current path: %s(pathlength: %ld)\n",
				pathchar, strlen(pathchar));
		else
			err_sys("getcwd error\n");
	}
	exit(EXIT_SUCCESS);
}