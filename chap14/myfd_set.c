/**
 * 一个可以记录2048个文件描述符的自定义fd_set
 */
#include <stdio.h>

typedef unsigned long __myfd_mask;

#define MYFDSETSIZE 2048							//比特位数
#define __NMYFDBITS (sizeof(__myfd_mask) * 8)	//myfd_set中每一个元素中含有的比特数
#define __MYFD_ELT(n) ((n) / __NMYFDBITS)		//给定比特位在myfd_set中的数组下标
#define __MYFD_MASK(n) (__myfd_mask)(1UL << ((n) % __NMYFDBITS))	//给定比特位的掩码

typedef struct {
	//内含32个元素
	__myfd_mask fd_bits[MYFDSETSIZE / __NMYFDBITS];
#define __FD_BITS(set) ((set)->fd_bits)		//给出myfd_set变量中数组指针
} myfd_set;


#define MYFD_ZERO(set) \
	for(int i = 0; i < MYFDSETSIZE / __NMYFDBITS; ++i) \
		__FD_BITS((set))[i] = 0;
#define MYFD_SET(set, fd) \
	((void) (__FD_BITS((set))[__MYFD_ELT((fd))] |= __MYFD_MASK((fd))) )
#define MYFD_CLR(set, fd) \
	((void) (__FD_BITS((set))[__MYFD_ELT((fd))] &= ~__MYFD_MASK((fd))) )
#define MYFD_ISSET(set, fd) \
	(__FD_BITS((set))[__MYFD_ELT((fd))] & __MYFD_MASK((fd)))


int main(void)
{
	myfd_set fset;
	MYFD_ZERO(&fset);

	if (MYFD_ISSET(&fset, 34))
		printf("fd 34 is set!\n");
	else
		printf("fd 34 is not set!\n");
	MYFD_SET(&fset, 34);
	if (MYFD_ISSET(&fset, 34))
		printf("fd 34 now is set!\n");
	else
		printf("fd 34 now is still not set!\n");
	MYFD_CLR(&fset, 34);
	if (MYFD_ISSET(&fset, 34))
		printf("fd 34 now is still set!\n");
	else
		printf("fd 34 now is clear!\n");

	return 0;
}