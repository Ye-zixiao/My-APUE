#ifndef OPEN0X32_H_
#define OPEN0X32_H_

#include "../../include/MyAPUE.h"

#define CL_OPEN "open"
#define CS_OPEN "/tmp/server.socket"
#define MAXLINE BUFSIZE

int csopen(char* pathname, int oflag);


#endif
