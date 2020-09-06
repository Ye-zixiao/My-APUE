CC:=gcc
cflags:=-Wall -lapue -Wl,-rpath,../lib -L../lib -lmyapue -lpthread \
		-lbsd -O  #-Werror
#-WL,-rpath,../lib -L../lib -lmyapue 的意思是到相对路径../lib中找动态链接库

SRCS:=makedetached.c mutex_robust.c recursive_func12.c norecursive_func12.c \
		recursive_retry.c flockfile.c mygetenv.c mygetenv_r.c pthread_key.c \
		pthread_sigmask.c atfork.c threadsort.c
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
