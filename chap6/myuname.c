#include"MyAPUE.h"
#include<sys/utsname.h>
#include<string.h>

#define _GNU_SOURCE


int main(void) {
	struct utsname utsbuf;
	if (uname(&utsbuf) != 0)
		err_sys("uname error\n");

	const char* domain = utsbuf.__domainname;
	printf("%s %s %s %s %s %s\n",
		utsbuf.sysname, utsbuf.nodename, utsbuf.release, utsbuf.version,
		utsbuf.machine, strcmp(domain,"(none)")==0?"":domain);

	exit(EXIT_SUCCESS);
}