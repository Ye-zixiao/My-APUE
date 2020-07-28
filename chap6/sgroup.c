#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<grp.h>

#define GRP_NUM 10

static struct group*
GetgrpUid(gid_t gid) {
	struct group* pret = NULL;

	setgrent();
	while ((pret = getgrent()) != NULL)
		if (pret->gr_gid == gid)
			break;
	endgrent();
	return pret;
}

int main(int argc, char* argv[])
{
	gid_t grparr[GRP_NUM];
	int mem_size = getgroups(GRP_NUM, grparr);
	if (mem_size > 0) {
		int i = 0;
		printf("group mem :");
		while (i < mem_size)
			printf(" %s", GetgrpUid(grparr[i++])->gr_name);
		putchar('\n');
	}
	
	exit(EXIT_SUCCESS);
}