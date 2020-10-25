#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>
#include<grp.h>

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

struct group*
	GetgrNam(const char* name) {
	struct group* p = NULL;
	
	setgrent();
	while ((p = getgrent()) != NULL)
		if (strcmp(name, p->gr_name) == 0)
			break;
	endgrent();
	return p;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_sys("usage:groupinfo <groupname>\n");

	struct group* pgrp = GetgrNam(argv[1]);
	if (pgrp == NULL)
		err_sys("GetgrNam error\n");
	printf("group name : %s\n", pgrp->gr_name);
	printf("group id   : %u\n", pgrp->gr_gid);
	if (pgrp->gr_mem != NULL) {
		char** pp = pgrp->gr_mem;
		printf("group mem  :");
		while(*pp!=NULL)
			printf(" %s", *pp++);
		putchar('\n');
	}

	exit(EXIT_SUCCESS);
}