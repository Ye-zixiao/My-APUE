/*
 * 这个程序要求的权限比较多，最好直接用root来进行实验
 * */
#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>
#include<shadow.h>

void err_sys(const char* msg);
struct swpd* GetspNam(const char* name);

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_sys("usage:usershadow <username>\n");

	struct spwd* p = GetspNam(argv[1]);
	if (p == NULL)
		err_sys("GetspNam error\n");
	printf("username    : %s\n", p->sp_namp);
	printf("hash passwd : %s\n", p->sp_pwdp);

	exit(EXIT_SUCCESS);
}


void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

struct swpd* GetspNam(const char* name) {
	struct spwd* pret = NULL;

	setspent();
	while ((pret = getspent()) != NULL)
		if (strcmp(name, pret->sp_namp) == 0)
			break;
	endspent();
	return pret;
}
