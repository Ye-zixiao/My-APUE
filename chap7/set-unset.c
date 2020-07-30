#include"MyAPUE.h"
#include<string.h>
#include<ctype.h>

char* StringUpper(char* buf, int size_len) {
	int i = 0;
	while (i < size_len + 1 || buf[i] == '\0') {
		buf[i] = toupper(buf[i]);
		++i;
	}
	return buf;
}

void print_env(const char* env) {
	const char* env_content = getenv(env);
	printf("%s = %s\n", env, env_content ? env_content : "");
}

int main(int argc, char* argv[]) {
	if (argc != 3)
		err_sys("usage:set-unset <env_var> <var_content>\n");

	const char* Upper_string = StringUpper(argv[1],strlen(argv[1]));
	if (setenv(Upper_string, argv[2], 0) == -1)
		err_sys("setenv error\n");
	printf("After setting:\n\t");
	print_env(Upper_string);
	if (unsetenv(Upper_string) == -1)
		err_sys("unsetenv error\n");
	printf("After unsetting:\n\t");
	print_env(Upper_string);

	exit(EXIT_SUCCESS);
}