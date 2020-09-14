/** 
 * 非阻塞IO
 */
#include "../include/MyAPUE.h"
#include <string.h>
#include <fcntl.h>

#define MAX 500000

char buf[MAX];

int main(void)
{
	char* ptr;
	int nread, nwrite;

	nread = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", nread);

	set_fl(STDOUT_FILENO, O_NONBLOCK);

	ptr = buf;
	while (nread > 0) {
		errno = 0;
		nwrite = write(STDOUT_FILENO, buf, nread);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);
		
		if (nwrite > 0) {
			ptr += nwrite;
			nread -= nwrite;
		}
	}

	clr_fl(STDOUT_FILENO, O_NONBLOCK);
	exit(EXIT_SUCCESS);
}
