#include"../include/MyAPUE.h"
#include<string.h>
#include<stdbool.h>

int main(int argc, char* argv[])
{
	bool timeflag = false;
	if (argc < 2)
		err_sys("usage:sleep <seconds> [-t(pr_now)]\n");
	else if (argc == 3 && strcmp(argv[argc - 1], "-t") == 0)
		timeflag = true;

	if (timeflag)
		pr_now();
	Sleep(atoi(argv[1]));
	if (timeflag)
		pr_now();

	exit(EXIT_SUCCESS);
}
