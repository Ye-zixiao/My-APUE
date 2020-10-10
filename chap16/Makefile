CC:=gcc
cflags:=-Wall -lapue -Wl,-rpath,../lib -L../lib -lmyapue -lpthread \
		-lbsd -lrt -O  #-Werror
SRCS:=
OBJS:=${SRCS:c=o}
PROGS:=${SRCS:.c=}

.PHONY:
all:${PROGS}

${PROGS}:%:%.o Makefile
	${CC} $< -o $@ $(cflags)

clean:
	rm -f ${PROGS} ${OBJS}

rmprg:
	rm -f ${PROGS}

rmobj:
	rm -f $(OBJS)

%.o:%.c Makefile
	${CC} ${cflags} -c $<
