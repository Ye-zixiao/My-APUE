#ifndef OPEND03RE_H_
#define OPEND03RE_H_

#include "../../include/MyAPUE.h"

#define CL_OPEN "open"
#define MAXLINE BUFSIZE

extern char errmsg[];
extern int oflag;
extern char* pathname;

void handle_request(char* buf, int nread, int fd);

#endif
