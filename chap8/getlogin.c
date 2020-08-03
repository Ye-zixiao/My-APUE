#include"MyAPUE.h"
#include<pwd.h>


int main(void)
{
	struct passwd* pwd;
	char* name;
	if((pwd=getpwuid(getuid()))==NULL)
		err_sys("getpwuid error\n");
	if ((name = getlogin()) == NULL)
		err_sys("getlogin error\n");
	printf("username(from getpwuid): %s\n", pwd->pw_name);
	printf("username(from getlogin): %s\n", name);

	exit(EXIT_SUCCESS);
}