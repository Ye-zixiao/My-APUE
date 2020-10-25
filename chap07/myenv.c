#include"MyAPUE.h"

extern char** environ;

int main(void)
{
	char* const * p = environ;
	for (; *p != NULL; ++p)
		printf("%s\n", *p);

	exit(EXIT_SUCCESS);
}