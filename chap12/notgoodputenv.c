#include"../include/MyAPUE.h"
#include<string.h>

extern char** environ;


inline static
size_t NameLen(const char* str) {
	size_t len = 0;

	for (; *str != '='; ++str) {
		if (*str == '\0')return 0;
		++len;
	}
	return len;
}


inline static 
size_t environSize(char* const* env) {
	size_t sz = 0;

	for (; env[sz] != NULL; ++sz);
	return sz;
}


int myputenv(const char* str) {
	char** newenv;
	char* newstr;
	size_t oldenvlen;
	size_t namelen;
	size_t strsz;

	if ((namelen = NameLen(str)) == 0)
		return -1;
	strsz = strlen(str);
	for (int i = 0; environ[i] != NULL; ++i) {
		/*
			若原本存在该变量
		*/
		if (strncmp(str, environ[i], namelen) == 0 &&
				environ[i][namelen] == '=') {
			if (strlen(environ[i]) >= strsz) {
				strcpy(&environ[i][namelen + 1], &str[namelen + 1]);
			}
			else {
				if ((newstr = malloc(sizeof(char) * (strsz + 1))) == NULL)
					return -1;
				strcpy(newstr, str);
				environ[i] = newstr;
			}
			return 0;
		}
	}

	/*
		若原本不存在该变量
	*/
	oldenvlen = environSize(environ);
	if ((newenv = malloc(sizeof(char*) * (oldenvlen + 2))) == NULL)
		return -1;
	if ((newstr = malloc(sizeof(char) * (strsz + 1))) == NULL) {
		free(newenv);
		return -1;
	}
	memcpy(newenv, environ, sizeof(char**) * oldenvlen);
	strcpy(newstr, str);
	newenv[oldenvlen] = newstr;
	newenv[oldenvlen + 1] = NULL;
	environ = newenv;
	return 0;
}


int main(void)
{
	myputenv("USER=MYDAD");
	myputenv("PATH=(null)");
	myputenv("SHOW=CODE");
	myputenv("LS_COLORS=(null)");
	myputenv("HELLO=WORLD");
	for (int i = 0; environ[i] != NULL; ++i)
		printf("%s\n", environ[i]);

	return 0;
}
