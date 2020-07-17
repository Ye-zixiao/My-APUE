CC:=gcc
cflags:=-Wall -Werror -lapue -O

SRCS:=3.1.c 3.2.c 3.5.c 3.11.c
OBJS:=${SRCS:c=o}
PROGS:=${SRCS:.c=}

.PHONY:
all:${PROGS}

${PROGS}:%:%.o Makefile
	${CC} $< -o $@ $(cflags)

clean:
	rm -f ${PROGS} ${OBJS}

%.o:%.c Makefile
	${CC} ${cflags} -c $<
