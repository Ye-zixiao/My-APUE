#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<alloca.h>

struct Type {
	char buf[64];
	int value;
};

void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int main(void)
{
	//struct Type* ptype = malloc(sizeof(struct Type));
	struct Type* ptype = calloc(sizeof(struct Type), 1);
	//struct Type* ptype = alloca(sizeof(struct Type));//alloca函数具有自动释放内存空间的作用
	if (strcpy(ptype->buf, "hello world") == NULL)
		err_sys("strcpy error\n");
	ptype->value = 10;

	printf("struct Type's value  = %d\n", ptype->value);
	printf("struct Type's string = %s\n", ptype->buf);

	free(ptype);
	exit(EXIT_SUCCESS);
}