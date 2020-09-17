CC:=gcc
cflags:=-Wall -lapue -Wl,-rpath,../lib -L../lib -lmyapue -lpthread \
		-lbsd -lrt -O  #-Werror
#-WL,-rpath,../lib -L../lib -lmyapue 的意思是到相对路径../lib中找动态链接库

SRCS:=unblockio.c mandatory.c advisory_lock.c select.c rot13_block.c \
		rot13_aio.c
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
