#include "opend.h"

#define NALLOC 10


/**
 * 为记录用户信息（uid+fd）的结构体动态数组分配或者重新分配
 * 内存，并且为其中未使用的元素进行初始化（置为无效状态）
 */
static void client_alloc(void) {
	if (client == NULL)
		client = malloc(NALLOC * sizeof(Client));
	else
		client = realloc(client, (NALLOC + client_size) * sizeof(Client));
	if (client == NULL)
		err_sys("can't alloc for client array");

	for (int i = client_size; i < client_size + NALLOC; ++i)
		client[i].fd = -1;
	client_size += NALLOC;
}


/**
 * 向用户信息结构体动态数组中添加一个新请求用户信息
 */
int client_add(int fd, uid_t uid) {
	if (client == NULL)
		client_alloc();

again:
	for (int i = 0; i < client_size; i++) {
		if (client[i].fd == -1) {
			client[i].fd = fd;
			client[i].uid = uid;
			return i;
		}
	}

	client_alloc();
	goto again;
}


/**
 * 从用户信息结构体动态数组中删除一个指定的用户信息
 */
void client_del(int fd) {
	for (int i = 0; i < client_size; i++) {
		if (client[i].fd == fd) {
			client[i].fd = -1;
			return;
		}
	}
	log_quit("can't find client entry for fd %d", fd);
}


int client_index(int fd) {
	for (int i = 0; i < client_size; ++i) {
		if (client[i].fd == fd)
			return i;
	}
	return -1;
}