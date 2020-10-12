#include "../include/MyAPUE.h"
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char* argv[])
{
	struct servent* pent;

	if (argc != 3)
		err_quit("usage: getserv <name> <protocol>");

	if ((pent = getservbyname(argv[1], argv[2])) == NULL)
		err_sys("getservbyname error");
	printf("NAME    :  %s\n", pent->s_name);
	printf("PORT    :  %d\n", ntohs(pent->s_port));
	printf("PROTOCOL:  %s\n", pent->s_proto);
	if (pent->s_aliases != NULL) {
		printf("ALIAS   :  ");
		for (int i = 0; pent->s_aliases[i]; ++i) {
			printf("%s ", pent->s_aliases[i]);
		}
		putchar('\n');
	}

	exit(EXIT_SUCCESS);
}
