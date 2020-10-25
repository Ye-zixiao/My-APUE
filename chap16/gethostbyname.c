/**
 * 使用已弃用的gethostbyname()函数对主机名进行地址解析，（
 * 当然我们在现代的编程中使用getaddrinfo()等函数来进行替代）
 */
#include "../include/MyAPUE.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{
	struct hostent* pent;
	char buf[INET_ADDRSTRLEN];

	if (argc != 2)
		err_quit("usage: %s <hostname>", argv[0]);

	if ((pent = gethostbyname(argv[1])) == NULL)
		err_sys("gethostbyname error");
	printf("hostname: %s\n", pent->h_name);
	if (pent->h_addrtype == AF_INET && pent->h_addr_list != NULL) {
		for (int i = 0; pent->h_addr_list[i] != NULL; ++i) {
			if (inet_ntop(AF_INET, (struct in_addr*)pent->h_addr_list[i],
					buf, INET_ADDRSTRLEN) == NULL)
				err_sys("inet_ntop error");
			printf("address : %s\n", buf);
		}
	}
	exit(EXIT_SUCCESS);
}

/*
实验结果：

$ ./main www.bilibili.com
hostname: www.bilibili.com
address : 120.92.169.231
address : 120.92.169.42
address : 112.25.54.211
address : 112.25.54.212
address : 119.3.74.154
address : 119.3.44.211

# yexinhai @ Localhost in ~/projects/APUE [20:22:59]
$ nslookup www.bilibili.com
Server:         192.168.137.1
Address:        192.168.137.1#53

Non-authoritative answer:
Name:   www.bilibili.com
Address: 112.25.54.212
Name:   www.bilibili.com
Address: 119.3.74.154
Name:   www.bilibili.com
Address: 119.3.44.211
Name:   www.bilibili.com
Address: 120.92.169.231
Name:   www.bilibili.com
Address: 120.92.169.42
Name:   www.bilibili.com
Address: 112.25.54.211
www.bilibili.com        canonical name = interface.biliapi.com.

*/
