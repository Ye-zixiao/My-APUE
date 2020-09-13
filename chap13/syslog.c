#include"../include/MyAPUE.h"
#include<syslog.h>

int main(void)
{
	setlogmask(LOG_UPTO(LOG_WARNING));
	errno = EACCES;
	openlog("test", LOG_PERROR, LOG_DAEMON);
	openlog("test notice", LOG_PERROR, LOG_DAEMON);
	syslog(LOG_ERR, "log from test: %m");
	syslog(LOG_INFO, "log from notice: %m");
	closelog();
	exit(EXIT_SUCCESS);
}
