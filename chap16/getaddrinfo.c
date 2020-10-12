/**
 * 由于getaddrinfo是gethostbyname和getservbyname函数的融合体，因
 * 此给定一个主机名(或加上服务名)，getaddrinfo会返回一个主机对应的
 * IP地址和服务所对应的端口号
 *	
 */
#include "../include/MyAPUE.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

/**
 * 地址族信息
 */
void print_family(const struct  addrinfo* aip) {
	printf(" family: ");
	switch (aip->ai_family) {
	case AF_INET:
		printf("INET");
		break;
	case AF_INET6:
		printf("INET6");
		break;
	case AF_UNIX:
		printf("UNIX");
		break;
	case AF_UNSPEC:
		printf("UNSPEC");
		break;
	default:
		printf("UNKNOWN");
	}
}


/**
 * 套接字类型
 */
void print_type(const struct addrinfo* aip) {
	printf(" type: ");
	switch (aip->ai_socktype) {
	case SOCK_STREAM:
		printf("STREAM");
		break;
	case SOCK_DGRAM:
		printf("DATAGRAM");
		break;
	case SOCK_SEQPACKET:
		printf("SEQPACKET");
		break;
	case SOCK_RAW:
		printf("RAW");
		break;
	default:
		printf("UNKNOWN (%d)", aip->ai_socktype);
	}
}


/**
 * 协议类型
 */
void print_protocol(const struct addrinfo* aip) {
	printf(" protocol: ");
	switch (aip->ai_protocol) {
	case 0:
		printf("default"); break;
	case IPPROTO_TCP:
		printf("TCP"); break;
	case IPPROTO_UDP:
		printf("UDP"); break;
	case IPPROTO_RAW:
		printf("RAW"); break;
	default:
		printf("UNKNOWN (%d)", aip->ai_protocol);
	}
}

/**
 * 返回的addrinfo中携带了何种信息（标志类型）
 */
void print_flags(const struct addrinfo* aip) {
	printf("flags:");
	if (aip->ai_flags == 0) {
		printf(" 0");
	}
	else {
		//套接字地址用于被动监听
		if (aip->ai_flags & AI_PASSIVE)
			printf(" PASSIVE");
		//规范名
		if (aip->ai_flags & AI_CANONNAME)
			printf(" CANON");
		//数字格式主机地址
		if (aip->ai_flags & AI_NUMERICHOST)
			printf(" NUMHOST");
		//服务以端口号表示
		if (aip->ai_flags & AI_NUMERICSERV)
			printf(" NUMSERV");
		//若没有IPv6地址则返回IPv4地址
		if (aip->ai_flags & AI_V4MAPPED)
			printf(" V4MAPPED");
		//查找所有的IP地址
		if (aip->ai_flags & AI_ALL)
			printf(" ALL");
	}
}


int main(int argc, char* argv[])
{
	char abuf[INET_ADDRSTRLEN];
	struct sockaddr_in* sinp;
	struct addrinfo* ailist;
	struct addrinfo hint;
	const char* addr;
	int err;
	
	if (argc != 3)
		err_quit("usage: %s nodename service", argv[0]);

	hint.ai_flags = AI_CANONNAME;
	hint.ai_family = AF_INET;
	hint.ai_socktype = 0;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_next = NULL;
	hint.ai_addr = NULL;
	if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));
	for (struct addrinfo* aip = ailist; aip != NULL; aip = aip->ai_next) {
		print_flags(aip);
		print_family(aip);
		print_type(aip);
		print_protocol(aip);
		printf("\n\thost: %s", aip->ai_canonname ? aip->ai_canonname : "-");
		if (aip->ai_family == AF_INET) {
			sinp = (struct sockaddr_in*)aip->ai_addr;
			addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf,
				INET_ADDRSTRLEN);
			printf(" address: %s", addr ? addr : "unknown");
			printf(" port: %d", ntohs(sinp->sin_port));
		}
		printf("\n");
	}

	exit(EXIT_SUCCESS);
}


/*
输入命令：

$ ./main baidu.com icmpd		#这个命令返回百度处理icmp消息服务的主机IP地址和端口信息
flags: CANON family: INET type: STREAM protocol: TCP
		host: baidu.com address: 220.181.38.148 port: 5813
flags: CANON family: INET type: DATAGRAM protocol: UDP
		host: - address: 220.181.38.148 port: 5813
flags: CANON family: INET type: STREAM protocol: TCP
		host: - address: 39.156.69.79 port: 5813
flags: CANON family: INET type: DATAGRAM protocol: UDP
		host: - address: 39.156.69.79 port: 5813

我们使用ping进行验证：
$ ping baidu.com
PING baidu.com (39.156.69.79) 56(84) 比特的数据。
64 比特，来自 39.156.69.79 (39.156.69.79): icmp_seq=1 ttl=51 时间=25.3 毫秒
64 比特，来自 39.156.69.79 (39.156.69.79): icmp_seq=2 ttl=51 时间=30.5 毫秒
64 比特，来自 39.156.69.79 (39.156.69.79): icmp_seq=3 ttl=51 时间=34.2 毫秒

确实该IP地址的主机是用来处理icmp消息的

*/
