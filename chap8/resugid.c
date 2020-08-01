#include"MyAPUE.h"
#include<pwd.h>
#include<grp.h>

int main(void)
{
	uid_t ruid, euid, suid;
	gid_t rgid, egid, sgid;
	struct passwd* ppw;
	struct group* pgrp;

	if (getresuid(&ruid, &euid, &suid) == -1)
		err_sys("getresuid error\n");
	if (getresgid(&rgid, &egid, &sgid) == -1)
		err_sys("getresgid error\n");
	if ((ppw = getpwuid(ruid)) == NULL)
		err_sys("getpwuid error\n");
	if ((pgrp = getgrgid(rgid)) == NULL)
		err_sys("getgrgid error\n");

	printf("username      : %s\n", ppw->pw_name);
	printf("group name    : %s\n", pgrp->gr_name);
	printf("real uid      : %d\n", ruid);
	printf("effective uid : %d\n", euid);
	printf("saved suid    : %d\n", suid);
	printf("real gid      : %d\n", rgid);
	printf("effective gid : %d\n", egid);
	printf("saved sgid    : %d\n", sgid);
	
	exit(EXIT_SUCCESS);
}

/*
下面的mtime是一个设置了suid的程序，它执行fork-exec组合生成一个子进程，其中exec函数设置了
其子进程的euid（0），并然后复制该euid给子进程的保留的suid（0）

yexinhai@localhost ~/m/chap8 (master)> ./mtime ./ugid
username      : yexinhai
group name    : yexinhai
real uid      : 1000
effective uid : 0
saved suid    : 0
real gid      : 1000
effective gid : 1000
saved sgid    : 1000

------------------------------
total time: 2757 us

而直接执行后面的ugid程序就不会这样了

yexinhai@localhost ~/m/chap8 (master)> ./ugid
username      : yexinhai
group name    : yexinhai
real uid      : 1000
effective uid : 1000
saved suid    : 1000
real gid      : 1000
effective gid : 1000
saved sgid    : 1000
yexinhai@localhost ~/m/chap8 (master)>
*/