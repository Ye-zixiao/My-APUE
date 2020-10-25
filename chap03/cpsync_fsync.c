#include"apue.h"
#include"Setfsf.h"
#include<fcntl.h>

#define BUFSIZE 4096

int main(void)
{
	int val;
	char buf[BUFSIZE];
	set_fsflag(STDOUT_FILENO, O_SYNC);
	while ((val = read(STDIN_FILENO, buf, BUFSIZE)) > 0) {
		if (write(STDOUT_FILENO, buf, val) != val)
			err_sys("write error");
		fsync(STDOUT_FILENO);
	}

	if (val < 0)err_sys("read error");
	exit(0);
}
