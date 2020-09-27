#include "../include/MyAPUE.h"
#include <ctype.h>

int main(void)
{
	int ch;

	while ((ch = getchar()) != EOF) {
		if (isupper(ch))
			ch = tolower(ch);
		if (putchar(ch) == EOF)
			err_sys("output error");
		if (ch == '\n')
			fflush(stdout);
	}

	exit(EXIT_SUCCESS);
}
