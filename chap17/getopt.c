/**
 * 目标：能够解析command [-i] [-u username] [-z] filename 
 * 命令行参数的程序
 */
#include "../include/MyAPUE.h"
#include <string.h>


int main(int argc, char* argv[])
{
	int i = 0, z = 0, ch;
	char* username = NULL, *filename = NULL;

	opterr = 0;
	while ((ch = getopt(argc, argv, "hiu:z")) != -1) {
		switch (ch) {
		case 'h':
			err_quit("usage: command [-i] [-u username] [-z] filename");
		case 'i':
			i = 1; break;
		case 'u':
			username = optarg;
			break;
		case 'z':
			z = 1; break;
		case '?':
			err_quit("unrecognized option: -%c", optopt);
		}
	}
	if (optind >= argc)
		err_quit("no input file");
	filename = argv[optind];

	printf("argv[] have:\n     %s %s%s %s filename: %s\n",
		i ? "i," : "", username ? "username: " : "",
		username ? username : "", z ? "z," : "",
		filename ? filename : "");
	exit(EXIT_SUCCESS);
}
