#include "opend.h"
#include <string.h>
#include <syslog.h>

extern int log_to_stderr;
int debug1, oflag, client_size;
char errmsg[MAXLINE];
Client* client = NULL;
char* pathname;


int main(int argc, char* argv[])
{
	int looptype = 1;
	int ch;

	log_open("open.server", LOG_PID, LOG_USER);

	opterr = 0;
	while ((ch = getopt(argc, argv, "hdse")) != -1) {
		switch (ch) {
		case 'h':
			err_quit("usage: %s [-d] [-s|e]\n"
				     "       %s [-h]\n"
			         "-d  for debug\n"
				     "-e  using epoll to loop\n"
				     "-s  using select to loop\n"
				     "-h  help", argv[0], argv[0]);
		case 'd':
			debug1 = log_to_stderr = 1;
			break;
		case 's':
			looptype = 0;
			break;
		case 'e':
			looptype = 2;
			break;
		case '?':
			err_quit("unrecognized option: -%c", optopt);
		}
	}

	if (debug1 == 0)
		daemonize("opend server");
	//looptype == 1 ? ploop() : looptype == 2 ? eloop() : sloop();
	if (looptype == 1)
		ploop();
	else if (looptype == 2)
		eloop();
	else
		sloop();
}
