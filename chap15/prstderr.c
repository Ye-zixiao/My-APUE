//供popen_tostderr.c测试使用
#include <stdio.h>

int main(void){
	fprintf(stderr, "hello world from stderr\n");
	return 0;	
}
