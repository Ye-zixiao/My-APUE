#include "../include/MyAPUE.h"
#include <arpa/inet.h>

inline void swap(char* lhs, char* rhs) {
	char tmp = *rhs;
	*rhs = *lhs;
	*lhs = tmp;
}

uint32_t myhtonl(uint32_t hostint32) {
	char* cptr = (char*)&hostint32;
	int low = 0, high = sizeof(uint32_t) - 1;

	while (low < high)
		swap(cptr + low++, cptr + high--);
	return hostint32;
}

int main(void)
{
	uint32_t value = 0x12345678;

	printf("normal order: 0x%x\n", value);
	printf("inet order  : 0x%x\n", htonl(value));
	printf("inet order2 : 0x%x\n", myhtonl(value));
	exit(EXIT_SUCCESS);
}
