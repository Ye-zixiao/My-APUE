#include"apue.h"
#include<sys/wait.h>

static void sig_int(int);

int main(void)
{
	char buf[MAXLINE];
	pid_t pid;
	int status;
	
	if(signal(SIGINT,sig_int)==SIG_ERR)
		err_sys("signal error");

	printf("%% ");
	while(fgets(buf,MAXLINE,stdin)!=NULL){
		//由于fgets调用会读取每次输入的h换行符所以需要将其替换，否则总是会fork失败
		if(buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1]='\0';

		if((pid=fork())<0)
			err_sys("fork error");
		else if(pid==0){
			execlp(buf,buf,(char*)NULL);
			err_ret("couldn't execute: %s",buf);
			exit(127);	
		}

		if((pid=waitpid(pid,&status,0))<0)
			err_sys("waitpid error");
		printf("%% ");
	}

	exit(0);
}

void sig_int(int signo){
	printf("interrupt\n%%\n");	
}
