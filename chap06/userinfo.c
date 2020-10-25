#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stddef.h>
#include<pwd.h>

struct passwd* GetpwNam(const char* name);
void err_sys(const char* msg);

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_sys("usage:userinfo <username>\n");

	struct passwd* pwd = GetpwNam(argv[1]);
	if (pwd == NULL)
		err_sys("GetpwNam error\n");
	printf("username: %s\n", pwd->pw_name);
	printf("uid     : %u\n", pwd->pw_uid);
	printf("gid     : %u\n", pwd->pw_gid);
	printf("home dir: %s\n", pwd->pw_dir);
	printf("shell   : %s\n", pwd->pw_shell);

	exit(EXIT_SUCCESS);
}


struct passwd*
GetpwNam(const char* name) {
	struct passwd* p = NULL;

	setpwent();
	while ((p = getpwent()) != NULL)
		if (strcmp(name, p->pw_name) == 0)
			break;
	endpwent();
	return p;
}

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}