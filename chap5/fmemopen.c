#include"apue.h"
#define BSZ 48

int main(void)
{
	FILE* fp;
	char buf[BSZ];

	memset(buf, 'a', BSZ - 2);					//填充buf中的前BSZ-2个字节
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)//将我们指定的buf作为“底层文件”进行打开，并返回文件指针，此时文件位置为0（因为被截断了）
		err_sys("fmemopen failed");
	printf("initial buffer contents: %s\n", buf);
	fprintf(fp, "hello, world");
	printf("before flush: %s\n", buf);
	fflush(fp);									//调用write系统调用，将该stdout buf中的数据刷新到内核的缓冲区高速缓存
	printf("after fflush: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'b', BSZ - 2);
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	fprintf(fp, "hello, world");
	fseek(fp, 0, SEEK_SET);						//相当于调用rewind
	printf("after fseek: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'c', BSZ - 2);
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	fprintf(fp, "hello, world");
	fclose(fp);									//关闭文件后，一定会调用write将stdout buf中的数据刷新到内核中的缓冲区高速缓存
	printf("after fclose: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	exit(EXIT_SUCCESS);
}