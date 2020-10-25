#ifndef OPEND03RE_H_
#define OPEND03RE_H_

#include "../../include/MyAPUE.h"

#define CL_OPEN "open"
#define CS_OPEN "/tmp/server.socket"
#define MAXLINE BUFSIZE


extern int debug1;
extern int oflag;
extern char errmsg[];
extern char* pathname;

typedef struct {
	int fd;
	uid_t uid;
} Client;


extern Client* client;//用户信息结构体数组指针
extern int client_size;//用户结构体数组大小（包括未使用的预留位）


int client_add(int fd, uid_t uid);
void client_del(int fd);
int client_index(int fd);

void sloop(void);
void ploop(void);
void eloop(void);

void handle_request(char* buf, int nread, int clifd, uid_t uid);


#endif
