CC:=gcc
cflags:=-Wall -lapue -Wl,-rpath,../lib -L../lib -lmyapue -lpthread \
		-lbsd -O  #-Werror
#-WL,-rpath,../lib -L../lib -lmyapue 的意思是到相对路径../lib中找动态链接库

SRCS:=thread_id.c pthread_exit.c thread_errno.c max.c thread_localreturn.c \
		pthread_cancel.c pthread_cleanup.c mutex_struct.c mutex_trylock.c \
		foo1.c foo2.c foo3.c mutex_timelock.c rwjobqueue.c pthread_cond.c \
		spinlock.c pthread_barrier.c
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
