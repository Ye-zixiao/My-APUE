#include "../include/MyAPUE.h"
#include <string.h>

#define MAXLINE 1024

int main(void)
{
	int val, int1, int2;
	char line[MAXLINE];

	while ((val = read(STDIN_FILENO, line, MAXLINE)) > 0) {
		line[val] = '\0';
		if (sscanf(line, "%d%d", &int1, &int2) == 2) {
			sprintf(line, "%d\n", int1 + int2);
			val = strlen(line);
			if (write(STDOUT_FILENO, line, val) != val)
				err_sys("write error");
		}
		else {
			if (write(STDOUT_FILENO, "invalid args\n", 14) != 14)
				err_sys("write error");
		}
	}

	exit(EXIT_SUCCESS);
}
