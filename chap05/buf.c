/*
* 用来检测stdin、stdout、stderr以及文件流的缓冲状态，缓冲区大小
* 
*/

#include<stdio.h>
#include<stdlib.h>
#define BUFSIZE 4096

void err_sys(const char* msg);
void print_streamstat(const char* ioname, FILE* fp);
int is_unbuffered(FILE* fp);
int is_linebuffered(FILE* fp);
int buffer_size(FILE* fp);


int main(int argc, char* argv[])
{
	FILE* fp;

	fputs("enter any character\n", stdout);
	if (getchar() == EOF)
		err_sys("getchar error\n");
	fputs("get one line to stderr\n", stderr);

	print_streamstat("stdin", stdin);//经过检测stdin竟然是全缓冲，我在实机上跑也是
	print_streamstat("stdout", stdout);//stdout也是全缓冲，前两者的buffer size都是1024
	print_streamstat("stderr", stderr);//确实无缓冲

	if ((fp = fopen("/etc/passwd", "r")) == NULL)
		err_sys("fopen error\n");
	//char buf[BUFSIZE];
	setvbuf(fp,NULL,_IOFBF,BUFSIZE);
	if (getc(fp) == EOF)
		err_sys("getc error\n");
	print_streamstat("file stream",fp);//全缓冲

	if (fclose(fp))
		err_sys("fclose error\n");
	exit(EXIT_SUCCESS);
}


void err_sys(const char* msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

void print_streamstat(const char* name, FILE* fp) {
	printf("stream =%s, ", name);
	if (is_unbuffered(fp))
		printf("unbuffered");
	else if (is_linebuffered(fp))
		printf("line buffered");
	else
		printf("fully buffered");
	printf(", buffer size = %d\n", buffer_size(fp));
}


int is_unbuffered(FILE* fp) {
	return fp->_flags & _IONBF;
}

int is_linebuffered(FILE* fp) {
	return fp->_flags & _IOLBF;
}

int buffer_size(FILE* fp) {
	return fp->_IO_buf_end -
		fp->_IO_buf_base;
}
