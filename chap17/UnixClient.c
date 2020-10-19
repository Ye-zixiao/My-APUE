/**
 * unix_clinet.c
 */
#include "../include/MyAPUE.h"
#include <string.h>

#define SERVER_PATH "/tmp/unix_server"

int main(int argc, char* argv[])
{
	int clifd;
	size_t len;

	if (argc != 2)
		err_quit("usage: unix_client <message>");

	len = strlen(argv[1]) + 1;
	if ((clifd = cli_conn(SERVER_PATH)) < 0)
		err_sys("cli_conn error");
	if (write(clifd, argv[1], len) != len)
		err_sys("write error");
	close(clifd);

	exit(EXIT_SUCCESS);
}
