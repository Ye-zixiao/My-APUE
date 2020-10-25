#include"MyAPUE.h"
#include<setjmp.h>

jmp_buf jump_buf;
static int value;

void in_func(void) {
	if (value < 2) {
		++value;
		longjmp(jump_buf, 1);
	}
	printf("get here?\n");
}

void func(void) {
	in_func();
}

int main(void)
{
	/*设置为volatile当遇到longjmp造成的非局部性goto不会回滚数据，
		但若没有volatile关键字就为其进行数据回滚，这个数据会在第
		一次调用setjmp时进行了记录*/
	volatile int value = 0;
	//int value = 0;

	if (setjmp(jump_buf) != 0)
		printf("from a deep function jump to here\n");
	++value;//finally 3?
	func();
	printf("value = %d\n", value);

	exit(EXIT_SUCCESS);
}
