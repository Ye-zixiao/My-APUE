#include "MyAPUE.h"
#include <string.h>
#include <pwd.h>

static void sig_handler(int sig) {
	struct passwd* ppwd;

	printf("in signal handler...\n");
	if ((ppwd = getpwnam("hll")) == NULL)
		err_sys("getpwnam error\n");
	alarm(1);
}


int main(void)
{
	struct passwd* ppwd;

	if (signal(SIGALRM, sig_handler) == SIG_ERR)
		err_sys("signal error\n");
	alarm(1);
	while (1) {
		if ((ppwd = getpwnam("yexinhai")) == NULL)
			err_sys("getpwnam error\n");
		if (strcmp("yexinhai", ppwd->pw_name) != 0)
			printf("return value corrupted! pw_name = %s\n",
				ppwd->pw_name);
	}
}
