#include "../include/MyAPUE.h"
#include <netdb.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{
	struct servent* psvent;
	
	if (argc != 3)
		err_quit("usage: %s <server> <protocol>", argv[0]);

	if ((psvent = getservbyname(argv[1], argv[2])) == NULL)
		err_sys("getservbyname error");
	printf("server name    : %s\n", psvent->s_name);
	printf("server port    : %d\n", ntohs(psvent->s_port));
	printf("server protocol: %s\n", psvent->s_proto);

	exit(EXIT_SUCCESS);
}
