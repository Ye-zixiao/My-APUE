/*
	可能需要root超级管理员编译执行才能成功
*/

#include"MyAPUE.h"
#include<string.h>
#include<shadow.h>

struct spwd*
	GetspNam(const char* name) {
	struct spwd* pret = NULL;
	setspent();
	while ((pret = getspent()) != NULL)
		if (strcmp(pret->sp_namp, name) == 0)
			break;
	endspent();
	return pret;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_sys("usage:spw <username>\n");

	struct spwd* ptr;
	if ((ptr = GetspNam(argv[1])) == NULL)
		err_sys("getspnam error\n");

	printf("hash passwd:\n%s\n", ptr->sp_pwdp == NULL || ptr->sp_pwdp[0] == '\0' ?
		"(none)" : ptr->sp_pwdp);

	exit(EXIT_SUCCESS);
}