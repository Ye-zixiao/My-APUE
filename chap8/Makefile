CC:=gcc
cflags:=-Wall -Werror -lapue -O

SRCS:=mypid.c fork.c vfork.c zombie.c with_cond.c race_cond.c \
		init_is_myparent.c mytime.c
OBJS:=${SRCS:c=o}
PROGS:=${SRCS:.c=}

.PHONY:
all:${PROGS}

${PROGS}:%:%.o Makefile
	${CC} $< -o $@ $(cflags)

clean:
	rm -f ${PROGS} ${OBJS}
	rm -f outfile

rmprg:
	rm -f ${PROGS}

rmobj:
	rm -f $(OBJS)

%.o:%.c Makefile
	${CC} ${cflags} -c $<
