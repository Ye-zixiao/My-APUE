/*
	多定时器功能：允许定时多个闹钟

	在书中apue中提到的方式记录在该链接中：http://www.kohala.com/start/libes.timers.txt
*/
#include"../include/MyAPUE.h"

/*
	设计的思想是：
		1、若alarm函数返回的值为0，表示之前未设置闹钟，则直接调用alarm函数，
			同时将函数指针放到timer_list的cur_pf，表示当前闹钟超时时需要调
			用的函数
		2、若alarm函数返回的值不为0，则应该以如下的方式进行思考（需要把alarm
			正在定时的时间也作为timer_list的头部，虽然实际不记录）：
			2.1、若新定时时间小于当前比较定时节点，则将新定时插入到当前节点的
				前面，而当前节点的时间保存两者差的绝对值
			2.2、若新定时时间大于当前比较定时节点，则当前节点不变，当新定时时
				间修改为两者差的绝对值，然后以递归的方式执行步骤2与当前节点的
				下一个节点比较
			2.3、特别的若当前节点是NULL，说明到了尾部，直接在尾部插入
		3、当超时发生时，进入SIGALRM信号处理程序，若timer_list非空，从首部取
			出一个新的节点，更新到cur_pf，并用其记录的定时调用alarm函数，然后
			调用此次定时的函数

*/


//定时节点
typedef struct node_t {
	unsigned second;
	void(*pf)(void);
	struct node_t* next;
}node_t;

//定时列表
typedef struct timer_list {
	void(*cur_pf)(void);
	node_t* head;
}timer_list;


//将定时列表定义在全局空间
static timer_list* plist;


/*
	定时列表初始化
*/
void timer_list_init(void) {
	plist = malloc(sizeof(timer_list));
	plist->cur_pf = NULL;
	plist->head = NULL;
}


/*
	销毁并释放定时列表
*/
void timer_list_free(void) {
	node_t* pnode = plist->head;
	while (pnode != NULL) {
		node_t* ptemp = pnode;
		pnode = pnode->next;
		free(ptemp);
	}
	free(plist);
}


/*
	判断定时列表是否为空
*/
int empty(void) {
	return plist->head == NULL;
}


/*
	以递归的方式决定将新的定时差插入到当前节点的前还是后
*/
void insert_sec(node_t** ppcur, unsigned second, void(*pf)(void)) {
	if (*ppcur == NULL) {
		*ppcur = malloc(sizeof(node_t));
		(*ppcur)->pf = pf;
		(*ppcur)->next = NULL;
		(*ppcur)->second = second;
	}
	else {
		if (second < (*ppcur)->second) {
			(*ppcur)->second -= second;
			node_t* ptemp = malloc(sizeof(node_t));
			ptemp->second = second;
			ptemp->next = *ppcur;
			ptemp->pf = pf;
			*ppcur = ptemp;
		}
		else if (second > (*ppcur)->second) {
			second -= (*ppcur)->second;
			insert_sec(&(*ppcur)->next, second, pf);
		}
	}
}


/*
	对insert_sec进行封装，向定时列表中插入一个新的定时差
*/
void insert(unsigned second, void(*pf)(void)) {
	insert_sec(&plist->head, second, pf);
}


/*
	定时列表头部弹出下一个定时差
*/
unsigned front_pop(void) {
	unsigned retsec;
	node_t* ptemp = plist->head;

	retsec = ptemp->second;
	plist->cur_pf = ptemp->pf;
	plist->head = ptemp->next;
	free(ptemp);
	return retsec;
}


/*
	SIGALRM的信号处理程序，没办法下一次的定时只能通过信号处理程序进行重定时
*/
static void sig_alrm(int signo) {
	void(*cur_pf)(void) = plist->cur_pf;

	if (!empty()) {
		alarm(front_pop());
	}
	if (cur_pf != NULL)
		cur_pf();
}


/*
	SIGQUIT的信号处理程序，负责在接收到推出信号时进行定时列表资源释放工作
*/
static void sig_quit(int signo) {
	timer_list_free();
	printf("\nquit\n");
	exit(0);
}


/*
	可多次定时的闹钟定时信号，重复定时同一个时刻的话以最早的那次为准
*/
void settimer(unsigned second, void(*pf)(void)) {
	unsigned remainsec;

	//这个signal不一定要放在这里，可以放在main函数中也可以
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal error\n");

	remainsec = alarm(0);
#ifdef DEBUG
	printf("second = %u, remainsec = %u\n", second, remainsec);
#endif // DEBUG

	if (remainsec == 0 && empty()) {
		alarm(second);
		plist->cur_pf = pf;
	}
	else if (remainsec == 0 && !empty()) {
		alarm(front_pop());
		settimer(second, pf);
	}
	else {
		if (second < remainsec) {//应该要直接插入，而不是借助insert函数
			alarm(second);
			node_t* ptemp = malloc(sizeof(node_t));
			ptemp->next = plist->head;
			ptemp->second = remainsec - second;
			ptemp->pf = plist->cur_pf;
			plist->head = ptemp;
			plist->cur_pf = pf;
		}
		else if (second > remainsec) {
			alarm(remainsec);
			insert(second - remainsec, pf);
		}
	} }


/*
* ----------------------main function--------------------------
*/
int main(void)
{
	timer_list_init();
	if (signal(SIGQUIT, sig_quit) == SIG_ERR)
		err_sys("signal error\n");

	pr_now();
	printf("begin(enter ^\\ to quit!):\n");
	printf("settimer: 8 5 7 2 4 -> 2 2 1 2 1\n");
	settimer(8, pr_now);
	settimer(5, pr_now);
	settimer(7, pr_now);
	settimer(2, pr_now);
	settimer(4, pr_now);
#ifdef DEBUG
	travel();
#endif // DEBUG

	while (1)
		pause();
}
