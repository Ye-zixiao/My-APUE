#include <stdio.h>

inline static
int endian_type(const int* value) {
	//1：小端字节序；0：大端字节序
	return ((*(char*)value) == 0x78) ? 1 : 0;
}

int main(void)
{
	int value = 0x12345678;
	int flag = endian_type(&value);

	printf("  #     MSB     LSB\n");
	printf("Value:   %x\n", value);
	printf("        %s\n", flag ?
		          "HADR   LADR" : "LADR   HADR");
	printf("Type : %s\n", flag ?
		          "little-endian" : "big-endian");

	return 0;
}
