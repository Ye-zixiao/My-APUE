CC=gcc
cflags=-fPIC

SRCS:=myerror.c proc.c state_n_date.c waittell.c
OBJS:=${SRCS:c=o}

all:${OBJS}
	${CC} -shared $(cflags) ${OBJS} -o libmyapue.so

%.o:%.c Makefile
	${CC} $(cflags) -c $<

.PHONY:
clean:
	rm -f ${OBJS} libmyapue.so

rmobj:
	rm -f ${OBJS} 
