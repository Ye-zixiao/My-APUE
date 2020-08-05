#include"MyAPUE.h"
#include<fcntl.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_sys("usage:cmd <filename>\n");

	pid_t pid;
	FILE* pf;
	if ((pf = fopen(argv[1],"a")) == NULL)
		err_sys("open error\n");
	if ((pid = vfork()) < 0)
		err_sys("vfork error\n");
	else if (pid == 0) {
		if (fputs("hello world from child\n", pf) == -1)
			err_sys("child fputs error\n");

		//1、Linux系统实现exit并没有关闭I/O流
		//exit(0);

		//2、子进程手动真的关闭了I/O流
		if (fclose(pf) == -1)
			err_sys("fclose error\n");
		_exit(0);
	}
	else {
		if (waitpid(pid, NULL, 0) != pid)
			err_sys("waitpid error\n");
		if (fputs("hello world from parent\n", pf) == -1)
			err_sys("parent fputs error\n");
	}

	exit(0);
}
