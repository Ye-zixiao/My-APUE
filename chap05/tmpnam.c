#include"apue.h"

int main(void)
{
	char name[L_tmpnam], line[MAXLINE];
	FILE* fp;

	printf("%s\n", tmpnam(NULL));
	tmpnam(name);
	printf("%s\n", name);

	if ((fp = tmpfile()) == NULL)
		err_sys("tmpfile error");
	fputs("one line of output\n", fp);
	rewind(fp);
	if (fgets(line, MAXLINE, fp) == NULL)
		err_sys("fgets error");
	fputs(line, stdout);

	fclose(fp);
	exit(EXIT_SUCCESS);
}