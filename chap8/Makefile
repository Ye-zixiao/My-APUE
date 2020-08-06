CC:=gcc
cflags:=-Wall -lapue -O #-Werror

SRCS:=mypid.c fork.c vfork.c zombie.c with_cond.c race_cond.c \
		init_is_myparent.c mytime.c noreturn.c resugid.c setreuid.c \
		setuid.c tell.c testinterp.c System.c tsys.c System_status.c\
		printreuid.c pracct.c getlogin.c nice.c times.c exec-time.c \
		vfork_exit.c vfork_funcreturn.c pr_exit.c siginfo.c do.c
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
