/*
* 这部分的实验最好在root给予setuids的权限下观察，程序的作用
* 是将当前进程的ruid保持不变，将euid、保留的suid设置为进程的
* ruid（若为超级用户执行）；否认普通用户只能修改euid
* */
#include"MyAPUE.h"

int main(void)
{
	print_resuid();
	//if (setreuid(getuid(), getuid()) != 0)
	/*
		不过如上所述的“超级用户可以将当前进程的ruid保持不变，将euid、保留的suid都设置为进程
		的ruid”这种情况只有在setreuid函数第一个参数不为-1的情况下发生，否则即使是超级用户也
		只能改变euid，保留的suid是不会跟随euid改变的。参考unix程序员手册英文原文：
			If the real user ID is being set (ruid is not −1), or the effective user ID is being set to a value not equal to the real user  ID,
			then the saved set-user-ID of the current process shall be set equal to the new effective user ID.
	*/
	if (setreuid(-1, getuid()) != 0)
		err_sys("setreuid error\n");
	print_resuid();

	exit(EXIT_SUCCESS);
}
