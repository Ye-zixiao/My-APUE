#include"MyAPUE.h"
#include<time.h>

int main(void)
{
	time_t realtime = time(NULL);
	printf("%s", ctime(&realtime));
	exit(0);
}