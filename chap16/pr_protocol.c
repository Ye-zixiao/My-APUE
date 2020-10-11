#include "../include/MyAPUE.h"
#include <netdb.h>

int main(void)
{
	struct protoent* pent;

	setprotoent(1);
	printf("        procotol           number            altername\n");
	while ((pent = getprotoent()) != NULL) {
		printf("%15s            %3d        %16s\n",
			pent->p_name, pent->p_proto,*pent->p_aliases);
	}
	endprotoent();

	exit(EXIT_SUCCESS);
}
