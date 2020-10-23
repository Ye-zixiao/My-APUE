/**
 * 我们的目的仍然是从上的字符串中"hello\tworld show\nme+your-code"
 * 分割出子字符串单词
 */
#include <stdio.h>
#include <string.h>

int main(void){
    char str[64] = "hello\tworld show\nme+your-code";
    char delim[6] = " \t\n-+";
    char *substr = NULL;

    substr = strtok(str, delim);
    if(substr == NULL)
        return 0;
    printf("%s\n", substr);
    while((substr = strtok(NULL, delim)) != NULL){
        printf("%s\n",substr);
    }

    return 0;
}
