#include "../include/MyAPUE.h"
#include <netdb.h>

int main(void)
{
	struct servent* pent;
	int i = 0;

	setservent(1);
	printf("     server            port\n");
	while ((pent = getservent()) != NULL) {
		printf("%15s        %5d\n", pent->s_name, pent->s_port);
		if (++i == 10)
			break;
	}
	endservent();

	exit(EXIT_SUCCESS);
}
